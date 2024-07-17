use crate::address;
use crate::errors::{CardanoError, R};
use crate::structs::SignVotingRegistrationResult;
use alloc::string::{String, ToString};
use alloc::vec::Vec;
use cardano_serialization_lib::address::{Address, BaseAddress};
use third_party::cryptoxide::hashing::blake2b_256;
use third_party::ed25519_bip32_core::{XPrv, XPub};
use third_party::hex;
use third_party::ur_registry::cardano::cardano_delegation::CardanoDelegation;
use third_party::ur_registry::cardano::governance::CardanoVotingRegistration;
use third_party::ur_registry::crypto_key_path::CryptoKeyPath;

pub fn build_metadata_cbor(
    delegations: Vec<CardanoDelegation>,
    stake_pub: &[u8],
    payment_address: &[u8],
    nonce: u64,
    voting_purpose: u8,
    entropy: &[u8],
    passphrase: &[u8],
) -> R<Vec<u8>> {
    match build_delegations(delegations, entropy, passphrase) {
        Ok(delegations_vec) => {
            let voting_registration = CardanoVotingRegistration::new(
                delegations_vec,
                hex::encode(stake_pub),
                hex::encode(payment_address),
                nonce,
                voting_purpose,
            );
            voting_registration
                .try_into()
                .map_err(|e: third_party::ur_registry::error::URError| {
                    CardanoError::InvalidTransaction(e.to_string())
                })
        }
        Err(e) => Err(e),
    }
}

pub fn build_delegations(
    delegations: Vec<CardanoDelegation>,
    entropy: &[u8],
    passphrase: &[u8],
) -> R<Vec<(String, u8)>> {
    let mut delegations_vec = Vec::new();
    for delegation in delegations {
        let vote_key = hex::encode(delegation.get_pub_key());
        delegations_vec.push((vote_key, delegation.get_weidth()));
    }
    Ok(delegations_vec)
}

pub fn sign(
    path: &String,
    delegations: Vec<CardanoDelegation>,
    stake_pub: &[u8],
    payment_address: &[u8],
    nonce: u64,
    voting_purpose: u8,
    entropy: &[u8],
    passphrase: &[u8],
) -> R<SignVotingRegistrationResult> {
    let cbor = build_metadata_cbor(
        delegations,
        stake_pub,
        payment_address,
        nonce,
        voting_purpose,
        entropy,
        passphrase,
    )?;
    sign_voting_registration(path, &cbor, entropy, passphrase)
}

pub fn sign_voting_registration(
    path: &String,
    unsigned: &[u8],
    entropy: &[u8],
    passphrase: &[u8],
) -> R<SignVotingRegistrationResult> {
    let icarus_master_key =
        keystore::algorithms::ed25519::bip32_ed25519::get_icarus_master_key_by_entropy(
            entropy, passphrase,
        )
        .map_err(|e| CardanoError::SigningFailed(e.to_string()))?;
    let bip32_signing_key =
        keystore::algorithms::ed25519::bip32_ed25519::derive_extended_privkey_by_xprv(
            &icarus_master_key,
            path,
        )
        .unwrap();
    let signed_data = bip32_signing_key.sign::<Vec<u8>>(&blake2b_256(unsigned));
    Ok(SignVotingRegistrationResult::new(
        signed_data.to_bytes().to_vec(),
    ))
}

pub fn parse_stake_address(stake_pub: Vec<u8>) -> R<String> {
    let stake_address =
        address::calc_stake_address_from_xpub(stake_pub.try_into().unwrap()).unwrap();
    Ok(stake_address)
}

pub fn parse_payment_address(payment_address: Vec<u8>) -> R<String> {
    let payment_address = Address::from_bytes(payment_address).unwrap();
    let addr = payment_address.to_bech32(None).unwrap();
    Ok(addr)
}

#[cfg(test)]
mod tests {
    use super::*;
    use third_party::ur_registry::crypto_key_path::PathComponent;

    #[test]
    fn test_sign_voting_registration() {
        let entropy = hex::decode("7a4362fd9792e60d97ee258f43fd21af").unwrap();
        let passphrase = b"";
        let path = "m/1852'/1815'/0'/2/0".to_string();
        let cbor = hex::decode("a119ef64a50181825820248aba8dce1e4b0a5e53509d07c42ac34f970ec452293a84763bb77359b5263f01025820ca0e65d9bb8d0dca5e88adc5e1c644cc7d62e5a139350330281ed7e3a6938d2c0358390069fa1bd9338574702283d8fb71f8cce1831c3ea4854563f5e4043aea33a4f1f468454744b2ff3644b2ab79d48e76a3187f902fe8a1bcfaad0418640500").unwrap();
        let sign_data_result =
            sign_voting_registration(&path, &cbor, &entropy, passphrase).unwrap();

        let hash = hex::encode(blake2b_256(&cbor));
        assert_eq!(
            hash,
            "d2c40028745e3aee415523cc492986147d39530a9bfdf60a15f54f1c023ce266".to_string()
        );
        assert_eq!(hex::encode(sign_data_result.get_signature()), "6adc7ca65cab8d2a7e4a918a95cde7bfe0a0f07c5a738de7476defe0389778a8708cb31c3f39db80c486532cc7437a4c5f299e9af2ce2f468723f793c5012609");
    }

    #[test]
    fn test_build_delegations() {
        let path1 = PathComponent::new(Some(1694), true).unwrap();
        let path2 = PathComponent::new(Some(1815), true).unwrap();
        let path3 = PathComponent::new(Some(0), true).unwrap();
        let item2_path3 = PathComponent::new(Some(1), true).unwrap();
        let path4 = PathComponent::new(Some(0), true).unwrap();
        let path5 = PathComponent::new(Some(0), true).unwrap();

        let source_fingerprint: [u8; 4] = [52, 74, 47, 03];
        let components = vec![path1, path2, path3, path4, path5];
        let components2 = vec![path1, path2, item2_path3, path4, path5];
        let crypto_key_path1: CryptoKeyPath =
            CryptoKeyPath::new(components, Some(source_fingerprint), None);
        let crypto_key_path2: CryptoKeyPath =
            CryptoKeyPath::new(components2, Some(source_fingerprint), None);

        let delegations = vec![
            CardanoDelegation::new(crypto_key_path1, 1),
            CardanoDelegation::new(crypto_key_path2, 2),
        ];
        let entropy = hex::decode("7a4362fd9792e60d97ee258f43fd21af").unwrap();
        let passphrase = b"";
        let delegations_vec = build_delegations(delegations, &entropy, passphrase).unwrap();

        assert_eq!(delegations_vec.len(), 2);
        assert_eq!(
            delegations_vec[0].0,
            "248aba8dce1e4b0a5e53509d07c42ac34f970ec452293a84763bb77359b5263f",
        );
        assert_eq!(delegations_vec[0].1, 1);
        assert_eq!(
            delegations_vec[1].0,
            "a89819a70d4e621bc5e0b7555abd787e5c71ef46bdb19c4f817af23c0f57dc10",
        );
        assert_eq!(delegations_vec[1].1, 2);
    }

    #[test]
    fn test_build_metadata_cbor_and_sign() {
        let path1 = PathComponent::new(Some(1694), true).unwrap();
        let path2 = PathComponent::new(Some(1815), true).unwrap();
        let path3 = PathComponent::new(Some(1), true).unwrap();
        let path4 = PathComponent::new(Some(0), true).unwrap();
        let path5 = PathComponent::new(Some(0), true).unwrap();

        let source_fingerprint: [u8; 4] = [52, 74, 47, 03];
        let components = vec![path1, path2, path3, path4, path5];
        let crypto_key_path1: CryptoKeyPath =
            CryptoKeyPath::new(components, Some(source_fingerprint), None);

        let delegations = vec![CardanoDelegation::new(crypto_key_path1, 1)];

        let entropy = hex::decode("7a4362fd9792e60d97ee258f43fd21af").unwrap();
        let passphrase = b"";
        let stake_pub =
            hex::decode("ca0e65d9bb8d0dca5e88adc5e1c644cc7d62e5a139350330281ed7e3a6938d2c")
                .unwrap();
        let payment_address = hex::decode("0069fa1bd9338574702283d8fb71f8cce1831c3ea4854563f5e4043aea33a4f1f468454744b2ff3644b2ab79d48e76a3187f902fe8a1bcfaad").unwrap();
        let nonce = 100;
        let voting_purpose = 0;
        let cbor = build_metadata_cbor(
            delegations,
            &stake_pub,
            &payment_address,
            nonce,
            voting_purpose,
            &entropy,
            passphrase,
        )
        .unwrap();

        assert_eq!(
            hex::encode(cbor.clone()),
            "a119ef64a50181825820a89819a70d4e621bc5e0b7555abd787e5c71ef46bdb19c4f817af23c0f57dc1001025820ca0e65d9bb8d0dca5e88adc5e1c644cc7d62e5a139350330281ed7e3a6938d2c0358390069fa1bd9338574702283d8fb71f8cce1831c3ea4854563f5e4043aea33a4f1f468454744b2ff3644b2ab79d48e76a3187f902fe8a1bcfaad0418640500",
        );

        let path = "m/1852'/1815'/0'/2/0".to_string();
        let sign_data_result =
            sign_voting_registration(&path, &cbor, &entropy, passphrase).unwrap();

        assert_eq!(hex::encode(sign_data_result.get_signature()), "4ec0df6a2bbcd79ff8632b228808391a7ba2078a24578698ac36a26bba23a9b0e3838ccdaa1d7be26a65d3cd20c7001df92aeb67dc9e7c4eada45fa17605b009");
    }

    #[test]
    fn test_sign() {
        let path = "m/1852'/1815'/0'/2/0".to_string();
        let entropy = hex::decode("7a4362fd9792e60d97ee258f43fd21af").unwrap();
        let passphrase = b"";

        let path1 = PathComponent::new(Some(1694), true).unwrap();
        let path2 = PathComponent::new(Some(1815), true).unwrap();
        let path3 = PathComponent::new(Some(1), true).unwrap();
        let path4 = PathComponent::new(Some(0), true).unwrap();
        let path5 = PathComponent::new(Some(0), true).unwrap();

        let source_fingerprint: [u8; 4] = [52, 74, 47, 03];
        let components = vec![path1, path2, path3, path4, path5];
        let crypto_key_path1: CryptoKeyPath =
            CryptoKeyPath::new(components, Some(source_fingerprint), None);

        let delegations: Vec<CardanoDelegation> = vec![CardanoDelegation::new(crypto_key_path1, 1)];
        let stake_pub =
            hex::decode("ca0e65d9bb8d0dca5e88adc5e1c644cc7d62e5a139350330281ed7e3a6938d2c")
                .unwrap();
        let payment_address = hex::decode("0069fa1bd9338574702283d8fb71f8cce1831c3ea4854563f5e4043aea33a4f1f468454744b2ff3644b2ab79d48e76a3187f902fe8a1bcfaad").unwrap();
        let nonce = 100;
        let voting_purpose = 0;

        let sign_data_result = sign(
            &path,
            delegations,
            &stake_pub,
            &payment_address,
            nonce,
            voting_purpose,
            &entropy,
            passphrase,
        )
        .unwrap();

        assert_eq!(hex::encode(sign_data_result.get_signature()), "4ec0df6a2bbcd79ff8632b228808391a7ba2078a24578698ac36a26bba23a9b0e3838ccdaa1d7be26a65d3cd20c7001df92aeb67dc9e7c4eada45fa17605b009");
    }

    #[test]
    fn test_address_calc() {
        let stake_pub =
            hex::decode("ca0e65d9bb8d0dca5e88adc5e1c644cc7d62e5a139350330281ed7e3a6938d2c")
                .unwrap();
        let stake_address = parse_stake_address(stake_pub).unwrap();
        assert_eq!(
            stake_address,
            "stake1uye6fu05dpz5w39jlumyfv4t082gua4rrpleqtlg5x704tg82ull2"
        );

        let payment_address_bytes = hex::decode("0069fa1bd9338574702283d8fb71f8cce1831c3ea4854563f5e4043aea33a4f1f468454744b2ff3644b2ab79d48e76a3187f902fe8a1bcfaad").unwrap();
        let payment_address = parse_payment_address(payment_address_bytes).unwrap();
        assert_eq!(payment_address, "addr_test1qp5l5x7exwzhgupzs0v0ku0censcx8p75jz52cl4uszr463n5nclg6z9gazt9lekgje2k7w53em2xxrljqh73gdul2ksx5mjej".to_string());
    }
}

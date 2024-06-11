use alloc::{string::String, vec::Vec};
use third_party::cryptoxide::{
    digest::Digest,
    hmac::Hmac,
    pbkdf2,
    sha2::{Sha256, Sha512},
};

pub(crate) fn pbkdf2_sha512(key: &[u8], salt: &[u8], iterations: u32, output: &mut [u8]) {
    let mut hmac = Hmac::new(Sha512::new(), key);
    pbkdf2::pbkdf2(&mut hmac, salt, iterations, output)
}

pub(crate) fn sha256(data: &[u8]) -> Vec<u8> {
    let mut hasher = Sha256::new();
    let mut out = [0u8; 32];
    hasher.input(data);
    hasher.result(&mut out);
    out.to_vec()
}

pub(crate) fn shorten_string(text: String) -> String {
    let mut _text = text.clone();
    if _text.len() > 512 {
        let _ = _text.split_off(512);
        _text.push_str("......");
    }
    return _text;
}

#[cfg(test)]
mod tests {
    use alloc::string::ToString;
    use super::shorten_string;
    extern crate std;

    #[test]
    fn test_shorten_string() {
        let text1 =
            "01020304050607080102030405060708010203040506070801020304050607080102030405060708";
        let result = shorten_string(text1.to_string());
        assert_eq!(result, "01020304050607080102030405060708......01020304050607080102030405060708");

        let text2 = "01020304";
        let result = shorten_string(text2.to_string());
        assert_eq!(result, "01020304");
    }
}

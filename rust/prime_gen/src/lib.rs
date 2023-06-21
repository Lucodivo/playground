// DISCLAIMER: No reason to generate primes when a hardcoded array would work just fine
// But this is a learning exercise, so I'm doing it the hard way

#[cfg(test)]
mod tests {
    #[test]
    fn gen_primes_less_than_correct_count() {
        let primes_under_100 = super::gen_primes_less_than(100);
        let primes_under_1000 = super::gen_primes_less_than(1000);

        assert_eq!(primes_under_100.len(), 25);
        assert_eq!(primes_under_1000.len(), 168);
    }

    #[test]
    fn is_prime_first_11_correct() {
        let primes_first_10 = super::gen_primes_less_than(12);

        assert!(!super::is_prime(0, &primes_first_10));
        assert!(!super::is_prime(1, &primes_first_10));
        assert!(super::is_prime(2, &primes_first_10));
        assert!(super::is_prime(3, &primes_first_10));
        assert!(!super::is_prime(4, &primes_first_10));
        assert!(super::is_prime(5, &primes_first_10));
        assert!(!super::is_prime(6, &primes_first_10));
        assert!(super::is_prime(7, &primes_first_10));
        assert!(!super::is_prime(8, &primes_first_10));
        assert!(!super::is_prime(9, &primes_first_10));
        assert!(!super::is_prime(10, &primes_first_10));
        assert!(super::is_prime(11, &primes_first_10));
    }

    #[test]
    fn gen_primes_first_n_correct_count_and_limits() {
        let primes_first_25 = super::gen_primes_first_n(25);
        let primes_first_168 = super::gen_primes_first_n(168);

        assert_eq!(primes_first_25.len(), 25);
        assert_eq!(primes_first_25[0], 2);
        assert_eq!(primes_first_25[24], 97);

        assert_eq!(primes_first_168.len(), 168);
        assert_eq!(primes_first_168[0], 2);
        assert_eq!(primes_first_168[167], 997);
    }
}

pub fn prime_factors(num: u32, primes: &[u32]) -> Vec<u32> {
    let mut prime_factors: Vec<u32> = Vec::new();

    let sqrt_num = (num as f64).sqrt() as u32;
    assert!(primes[primes.len() - 1] >= sqrt_num, "Primes vector isn't long enough to find all prime factors of {}.", num);
    for prime in primes.iter() {
        if *prime > sqrt_num {
            break;
        } else if num % prime == 0 {
            prime_factors.push(*prime);
        }
    }
    return prime_factors;
}

pub fn is_prime(num: u32, primes: &[u32]) -> bool {
    if num < 2 { 
        false 
    } else { 
        prime_factors(num, primes).is_empty() 
    }
}

pub fn gen_primes_first_n(num_primes: u32) -> Vec<u32> {
    let mut primes: Vec<u32> = Vec::new();
    if num_primes == 0 { return primes; }
    
    primes.push(2);
    let mut num: u32 = 3;

    while primes.len() < num_primes as usize && num < std::u32::MAX  {
        let sqrt_num = (num as f64).sqrt() as u32;
        for prime in primes.iter() {
            if *prime > sqrt_num {
                primes.push(num);
                break;
            } else if num % prime == 0 {
                break;
            }
        }
        num += 2; // minor optimization, no need to check even numbers
    }

    return primes;
}

pub fn gen_primes_less_than(max_num: u32) -> Vec<u32> {
    let mut primes: Vec<u32> = Vec::new();
    if max_num < 2 { return primes; }
    
    primes.push(2);
    let mut num: u32 = 3;

    while num < max_num  {
        let sqrt_num = (num as f64).sqrt() as u32;
        for prime in primes.iter() {
            if *prime > sqrt_num {
                primes.push(num);
                break;
            } else if num % prime == 0 {
                break;
            }
        }
        num += 2; // minor optimization, no need to check even numbers
    }

    return primes;
}

pub fn print_primes_first_n(num_primes: u32) {
    if num_primes == 0 { return; }

    let primes = gen_primes_first_n(num_primes);

    let mut primes_iter = primes.iter();
    print!("{}", primes_iter.next().unwrap());

    for prime in primes_iter {
        print!(", {}", prime);
    }

    println!();
}

pub fn print_primes_less_than(max_num: u32) {
    if max_num < 2 { return; }

    let primes = gen_primes_less_than(max_num);

    let mut primes_iter = primes.iter();
    print!("{}", primes_iter.next().unwrap());

    for prime in primes_iter {
        print!(", {}", prime);
    }

    println!();
}
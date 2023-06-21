use prime_gen;

#[allow(dead_code)]
fn euclids_theorem_doesnt_produce_only_primes() {
    let primes = prime_gen::gen_primes_first_n(100);
    
    let mut product = 1;
    for (index, prime) in primes.iter().enumerate() {
        product = product * prime;
        let product_plus_one = product + 1;
        if !prime_gen::is_prime(product_plus_one, &primes) {
            let prime_factors = prime_gen::prime_factors(product_plus_one, &primes);
            println!("{} is the product of sequential primes {:?} + 1 and is factored by the primes {:?}", product_plus_one, &primes[..=index], prime_factors);
            break;
        }
    }
}

#[allow(dead_code)]
fn gen_n_primes() {
    let num_primes = 1_000;
    let primes = prime_gen::gen_primes_less_than(num_primes);
    println!("Primes less than {} count: {}", num_primes, primes.len());
    println!("Primes less than {}: {:?}", num_primes, primes);
}

fn main() {
    euclids_theorem_doesnt_produce_only_primes();
}

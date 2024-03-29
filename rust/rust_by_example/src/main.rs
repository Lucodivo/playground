fn main() {
    conditional_compilation();
}


#[allow(dead_code)]
fn conditional_compilation() {
    // All the following features must be specified in Cargo.toml
    // To be compiled by...

    // cargo build --features "mode_1"
    #[cfg(feature = "mode_1")]
    fn conditional_function() {
        println!("1");
    }

    // cargo build --features "mode_2"
    #[cfg(feature = "mode_2")]
    fn conditional_function() {
        println!("2");
    }

    #[cfg(not(any(feature = "mode_1", feature = "mode_2")))]
    fn conditional_function() {
        println!("other");
    }

    conditional_function();

    #[cfg(feature = "mode_1")]
    let feature_name = "mode_1";

    #[cfg(feature = "mode_2")]
    let feature_name = "mode_2";

    #[cfg(feature = "other")]
    let feature_name = "other";
    
    #[cfg(any(feature = "mode_1", feature = "mode_2", feature = "other"))]
    println!("feature_name: {}", feature_name);
    
    #[cfg(feature = "default")]
    println!("Default runs no matter the build but \"default\" must be a specified feature in Cargo.toml");
    #[cfg(not(feature = "default"))]
    println!("This never gets printed as far as I can tell.");
}

#[allow(dead_code)]
fn mutability_and_copy() {
    // Structs move when they don't implement copy
    {
        struct Point { x: f64, y: f64 }
    
        let p = Point { x: 1.0, y: 2.0 };
        println!("p.x: {}, p.y: {}", p.x, p.y);
    
        let mut p_mut = p;
        p_mut.x = 3.0;
        // println!("p.x: {}, p.y: {}", p.x, p.y); // Note: Won't compile. p was moved, *not* copied, to p_mut. p is now invalid.
        println!("p_mut.x: {}, p_mut.y: {}", p_mut.x, p_mut.y);
    }
    
    // Structs copy when they implement copy
    {
        #[derive(Clone, Copy)] // Note: `pub trait Copy: Clone`
        struct PointCopyable { x: f64, y: f64 }

        let p = PointCopyable { x: 1.0, y: 2.0 };
        println!("p.x: {}, p.y: {}", p.x, p.y);

        let mut p_mut = p;
        p_mut.x = 3.0;
        println!("p.x: {}, p.y: {}", p.x, p.y); // Note: Compiles just fine. p was copied, *not* moved, to p_mut. p is still valid.
        println!("p_mut.x: {}, p_mut.y: {}", p_mut.x, p_mut.y);
    }
}

#[allow(dead_code)]
fn owndership_and_destructuring() {
    struct Point{ x: f64, y: f64 }

    let mut p = Point { x: 0.0, y: 0.0 };
    p.x = 1.1;
    let Point { x: ref x_ref, y: ref mut y_ref } = p;

    // p.x = 2.2; Note: Won't compile. Immutable borrow x_ref still active at this point.
    // *x_ref += 1.1; // Note: Won't compile. x_ref is immutable.
    *y_ref = 3.3;
    println!("x_ref: {}, y_ref: {}", x_ref, y_ref);

    // Note: The active deconstructed mutable ref for y does not prevent p.x from being modified.
    p.x = 2.2;
    println!("p.x: {}, p.y: {}", p.x, y_ref);
}

#[allow(dead_code)]
fn mutability_and_borrowing() { 

    fn borrow(mut heap_data_function_borrow: Box::<u64>) {
        *heap_data_function_borrow = 20;
        println!("heap_data_function_borrow: {}", heap_data_function_borrow);
    }

    // memory obtained
    let heap_data_original = Box::new(5u64);
    println!("heap_data: {}", heap_data_original);

    // memory transferred
    let mut heap_data_mutable_borrow = heap_data_original;
    println!("heap_data_mutable_borrow: {}", heap_data_mutable_borrow);
    // println!("heap_data_original: {}", heap_data_original); // Note: Won't compile. Original variable pointing to memory is invalid.
    
    // memory modified
    *heap_data_mutable_borrow = 10;
    println!("heap_data_mutable_borrow: {}", heap_data_mutable_borrow);

    // memory moved to function argument
    borrow(heap_data_mutable_borrow);
    // println!("heap_data_original: {}", heap_data_original); // Note: Won't compile. Mutable borrow variable has been moved to fn and is now invalid.
}

#[allow(dead_code)]
fn multiple_references() {
    // immutable box with immutable references
    let x = Box::new(5u64);
    let y = &x;
    let z = &y;
    println!("x: {}, y: {}, z: {}", x, y, z);

    // Note: Won't compile. Immutable. Cannot get mutable reference to immutable Box<u64>
    // *x += 1;
    // let _w = &mut x; 
    
    // Move x to be a mutable under the same variable name and mofify the value.
    let mut x = x;
    *x += 1;
    println!("x: {}", x);

    // Gain mutable reference to mutable Box<u64> and modify the value.
    let w = &mut x;

    // Note: Won't compile.
    // println!("x: {}", x);
    
    **w += 1; // '**' -> Dereference mutable reference, dereference Box<u64>
    println!("w: {}", w);
}

#[cfg(test)]
mod tests {
    #[test]
    fn equality_derefs_ref() {
        let a = 0;
        let b = 0;
        assert_eq!(&a, &b);
        assert_eq!(&&a, &&b);
        assert_eq!(&&&a, &&&b);
        assert_eq!(&&&&a, &&&&b);
    }

    #[test]
    fn equality_for_refs() {
        let a = 0u64;
        let b = 0u64;
        let a_ref = &a;
        let b_ref = &b;

        // all of these dereference
        assert_eq!(&a, &b);
        assert_eq!(a_ref, b_ref);
        assert_eq!(a_ref, &a);
        assert_eq!(a_ref, &b);
        assert_eq!(b_ref, &a);
        assert_eq!(b_ref, &b);

        // comparing references as pointers
        assert_ne!(a_ref as *const u64, &b);
        assert_ne!(a_ref as *const u64, &b as *const u64);
        assert_ne!(b_ref as *const u64, &a);
        assert_ne!(b_ref as *const u64, &a as *const u64);
        assert_eq!(a_ref as *const u64, &a);
        assert_eq!(a_ref as *const u64, &a as *const u64);
        assert_eq!(b_ref as *const u64, &b);
        assert_eq!(b_ref as *const u64, &b as *const u64);
    }

    #[test]
    fn lazy_counter() {

        fn COUNTER() -> u64 {
            static mut COUNTER: u64 = 0;
            unsafe {
                COUNTER += 1;
                COUNTER
            }
        }

        use once_cell::sync::Lazy;

        static LAZY_ONE: Lazy<u64> = Lazy::new(|| { COUNTER() });
        static LAZY_TWO: Lazy<u64> = Lazy::new(|| { COUNTER() });
        static LAZY_THREE: Lazy<u64> = Lazy::new(|| { COUNTER() });
        static LAZY_FOUR: Lazy<u64> = Lazy::new(|| { COUNTER() });

        assert_eq!(*LAZY_ONE, 1); // initialized here
        assert_eq!(*LAZY_THREE, 2); // initialized here
        assert_eq!(*LAZY_FOUR, 3); // initialized here
        assert_eq!(*LAZY_TWO, 4); // initialized here

        assert_eq!(*LAZY_ONE, 1);
        assert_eq!(*LAZY_TWO, 4);
        assert_eq!(*LAZY_THREE, 2);
        assert_eq!(*LAZY_FOUR, 3);
    }
}
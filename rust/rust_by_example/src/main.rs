fn main() {
    mutability_and_copy();
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
fn main() {
    let mut s1 = String::from("run");
    let s2 = &mut s1;
    println!("{}", s2);
    
    s2.push_str("hx");
    println!("{}", s2);
    println!("{}",s1);
}
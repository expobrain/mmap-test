extern crate mmap;
extern crate time;

use std::os::unix::io::AsRawFd;
use std::path::Path;
use std::fs;
use mmap::{MapOption, MemoryMap};

const FILE_SIZE: usize = 256 * 1024 * 1024;
const VALUE_SIZE: usize = 2; //u16
const ARRAY_SIZE: usize = 16;

fn read(items: &&[[u16; 16]], items_count: usize) {
    let mut counter: u64;
    let start_ts: u64;
    let end_ts: u64;
    let delta_ts: u64;

    counter = 0;
    start_ts = time::precise_time_ns();

    for i in 0..items_count {
        for c in 0..ARRAY_SIZE {
            let value = items[i][c];
            counter += value as u64;
        }
    }

    end_ts = time::precise_time_ns();
    delta_ts = end_ts - start_ts;

    println!(
        "{}.{:09}",
        delta_ts / 1_000_000_000,
        delta_ts - delta_ts / 1_000_000_000 * 1_000_000_000
    );
    println!("{}", counter);
}

fn main() {
    let path = Path::new("array.data");
    let f = fs::File::open(&path).unwrap();

    let mmap_opts = &[MapOption::MapReadable, MapOption::MapFd(f.as_raw_fd())];

    let mmap = MemoryMap::new(FILE_SIZE, mmap_opts).unwrap();

    let data = mmap.data() as *const [u16; 16];

    if data.is_null() {
        panic!("Could not access data from memory mapped file")
    }

    let items_count = FILE_SIZE / (VALUE_SIZE * ARRAY_SIZE);
    let items: &[[u16; 16]] = unsafe { std::slice::from_raw_parts(data, items_count) };

    read(&items, items_count);
    read(&items, items_count);
}

/*
 * Copyright (c) 2021, WSO2 Inc. (http://www.wso2.org) All Rights Reserved.
 *
 * WSO2 Inc. licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

// Rust Library

use std::collections::HashMap;
use std::ffi::CStr;
use std::mem;
use std::os::raw::c_char;

// String comparision
#[no_mangle]
pub extern "C" fn is_same_type(src_type : *const c_char, dest_type : *const c_char) -> bool
{
  return src_type == dest_type;
}

// Prints 64 bit signed integer
#[no_mangle]
pub extern "C" fn print64(num64 : i64)
{
   println!("{}", num64);
}

// Prints 32 bit signed integer
#[no_mangle]
pub extern "C" fn print32(num32 : i32)
{
   println!("{}", num32);
}

// Prints 16 bit signed integer
#[no_mangle]
pub extern "C" fn print16(num16 : i16)
{
   println!("{}", num16);
}

// Prints 8 bit signed integer
#[no_mangle]
pub extern "C" fn print8(num8 : i8)
{
   println!("{}", num8);
}

// Prints 64 bit unsigned integer
#[no_mangle]
pub extern "C" fn printu64(num64 : u64)
{
   println!("{}", num64);
}

// Prints 32 bit unsigned integer
#[no_mangle]
pub extern "C" fn printu32(num32 : u32)
{
   println!("{}", num32);
}

// Prints 16 bit unsigned integer
#[no_mangle]
pub extern "C" fn printu16(num16 : u16)
{
   println!("{}", num16);
}

// Prints 8 bit unsigned integer
#[no_mangle]
pub extern "C" fn printu8(num8 : u8)
{
   println!("{}", num8);
}

// Prints 64 bit float
#[no_mangle]
pub extern "C" fn printf64(num64 : f64)
{
   println!("{}", num64);
}

// Prints 32 bit float
#[no_mangle]
pub extern "C" fn printf32(num32 : f32)
{
   println!("{}", num32);
}

#[no_mangle]
pub extern "C" fn new_int_array(size: i32) -> *mut Vec<*mut i32> {
    let mut size_t = size;
    if size < 0 {
        size_t = 8;
    }
    let size_t = size_t as usize;
    let foo: Box<Vec<*mut i32>> = Box::new(Vec::with_capacity(size_t));
    let vec_pointer = Box::into_raw(foo);
    return vec_pointer as *mut Vec<*mut i32>;
}

#[no_mangle]
pub extern "C" fn int_array_store(arr_ptr: *mut Vec<*mut i32>, n: i32, ref_ptr: *mut i32) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let n_size = n as usize;
    let len = n_size + 1;
    if arr.len() < len {
        arr.resize(len, 0 as *mut i32);
    }
    arr[n_size] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn int_array_load(arr_ptr: *mut Vec<*mut i32>, n: i32) -> *mut i32 {
    let arr = unsafe { Box::from_raw(arr_ptr)};
    let n_size = n as usize;
    let return_val = arr[n_size];
    mem::forget(arr);
    return return_val;
}

// Ballerina Map implementation
// ref: http://jakegoulding.com/rust-ffi-omnibus/objects/

pub struct BalMapInt {
   map: HashMap<String, i32>,
}

impl BalMapInt {
   fn new() -> BalMapInt {
      let mut val = BalMapInt {
         map: HashMap::new(),
      };
      val.test_init();
      return val;
   }

   fn test_init(&mut self) {
      self.map.insert(String::from("test_key"), 42);
   }
   fn get(&self, key: &str) -> i32 {
      self.map.get(key).cloned().unwrap_or(0)
   }

   fn get_length(&self) -> usize {
      self.map.len()
   }

   fn insert_field(&mut self, key: &str, member: i32) {
      self.map.insert(String::from(key), member);
   }
}

#[no_mangle]
pub extern "C" fn map_new_int() -> *mut BalMapInt {
   println!("map_new_int");
   Box::into_raw(Box::new(BalMapInt::new()))
   // let foo: Box<Vec<*mut i32>> = Box::new(Vec::with_capacity(size_t));
   // let vec_pointer = Box::into_raw(foo);
}

#[no_mangle]
pub extern "C" fn map_deint_int(ptr: *mut BalMapInt) {
   if ptr.is_null() {
      return;
   }
   unsafe {
      Box::from_raw(ptr);
   }
}

#[no_mangle]
pub extern "C" fn map_store_int(ptr: *const BalMapInt, key: *const c_char, member_ptr: *const i32) {
   println!("map_store_int");
   let bal_map = unsafe {
      assert!(!ptr.is_null());
      &*ptr
   };
   let key = unsafe {
      assert!(!key.is_null());
      CStr::from_ptr(key)
   };
   let key_str = key.to_str().unwrap();
   println!("{}", key_str);
   println!("map length {}", bal_map.get_length());
}

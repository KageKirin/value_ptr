# Purpose

`value_ptr` aims to provide a class similar to `unique_ptr`, but with copy semantics of the pointed-to object.

This allows two things:

1. You can give an object that would otherwise be expensive to move cheap move semantics at the cost of an extra indirection, but otherwise remain unchanged. For instance, an application that stores many of a particular class in a container and needs to perform frequent sorting and insertions may find a `std::vector<value_ptr<T>>` to perform better than `std::vector<T>`, `std::list<T>`, or `std::deque<T>`.
2. You can treat an object like a value, but still use run-time polymorphism via virtual function calls.

Unfortunately, C++ does not allow overloading `operator.` (the member access operator), so the syntax requires pointer syntax of `value->` rather than value syntax of `value.`, even though `value_ptr` models the latter.

# Implementation / Design Decisions

Wherever possible, `value_ptr` defers to `unique_ptr` in its implementation, and seeks only to concern itself with copy construction and copy assignment.

The definition of a copy can be surprisingly challenging, however. For any type that isn't a class, the copy constructor / copy assignment operator of the static type will always be correct, because only classes can serve as a base of another type. However, for a class type, I can have a situation such as `value_ptr<base_type>`, where the static type is `base_type`, but it may actually be pointing to an object that is a `derived_type`. In this case, if I were to just use the copy constructor of `base_type`, I would 'slice' the object that I'm trying to copy.

## Cloning policy

### Other implementations

#### member clone

This situation has been solved in a few different ways. Some libraries that deal with copying polymorphically require the class to define a virtual `clone` member function in the base and all derived classes. This has one major drawback, however.

* It requires the contained class to be modified to use it with the `value_ptr`. This can be impossible in the case that the pointed-to object comes from third-party code.

Note that it does not require a virtual function. The usage of a virtual member function `clone` vs. a non-virtual member function `clone` is identical. A class that is not virtual and is not designed to be derived from can have a regular member function that just calls the copy constructor.


#### Template function

Other libraries use a template function that must be defined for the user's type. This is the approach used by the Boost.Pointer Container library. This neatly avoids the problem listed above, and allows for easy specialization of non-class types. The Boost Pointer Container library uses the function `new_clone`:

However, the question remains of what the default behavior should be.

Boost.Pointer Container chose to define a default copy for all types, which is to invoke the copy constructor of the static type. However, if you use it in such a way that it would cause a slice, there is a runtime error.

TODO: Determine if there is a runtime error or a slice for the default implementation in the event that you try to copy a base class pointer that has no virtual member functions.

### This library's implementation

One drawback with the `new_clone` approach is that you are defining cloning behavior on a per-type basis. This is correct for a majority of use cases (for instance, a polymorphic base that has a virtual clone member function should always be cloned used that member function), but it fails to support use cases that `std::unique_ptr` works with.

The most straightforward example is interoperability with C functions. A user of `value_ptr` may wish to, in some cases, transfer ownership of the pointer to some C function. The only safe way to do this is to tell the `value_ptr` that in this case, the object should be allocated with `malloc` and deleted with `free`. However, the class owned by the `value_ptr` would probably still want default behavior of `new` and `delete`.

Due to the above reasons, `value_ptr` has a template parameter that defines the cloning strategy, similar to how `std::unique_ptr` allows for custom deleters. A default definition is given for non-class types that performs a simple copy, and this definition can be expanded to class types by specializing `default_new`.

One possible option for the future would be to use `std::allocator` rather than `default_new`.

# Prior work

## Edd Dawson's value_ptr

[http://www.mr-edd.co.uk/code/value_ptr]

This class fails to follow the zero-overhead principle. `std::unique_ptr`'s guiding design was that it should have no run-time overhead in space or time compared to using a raw pointer. Dawson's `value_ptr` has virtual functions and two pointers of overhead.

## Martinho Fernandes's Wheels value_ptr

[https://bitbucket.org/martinhofernandes/wheels/src/8a1366b0a195/include/wheels/smart_ptr/value_ptr.h%2B%2B?at=default]

I found this when my `value_ptr` was nearly complete, and it is almost identical to my version, down to many implementation details. It appears to follow the no-overhead principle, and has a template parameter for the cloner and deleter. However, I believe my implementation to be slightly simpler, because my `value_ptr` defers to `std::unique_ptr` wherever possible, whereas Fernandes's `value_ptr` implements everything itself. However, there is a lot of good stuff in this version, and I expect that my value_ptr will be updated with some improvements that Fernandes has.

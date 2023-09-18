# Implementation notes

## Invalidate/Draw

In BeOS BView::Invalidate() sends message to app_server requesting region update.
We are doing drawing in-process, so we send _UPDATE_ message to BView instead.
It is handled as a BView::Draw() call in a clipped Skia Canvas rectangle.
There is one Skia Canvas per window, exposed by private accessor function.

## Private Implementation

We do not make shared private implementations. Every class contains its pimpl in
its own .cpp file. If there is a need to expose parts of private implementation
it is done via private accessor functions exposing pointers to daclaration-only
class. This allows friend class to #include full class implementation header
and use the exposed pointer directly.

Private non-virtual accessor functions do not change class ABI, so we are free
to add/remove these as needed.

## Code style

- Private methods are prefixed with `_`
- Method out-parameters are prefixed with `_` in implementation

base4k
======

Base4k is an encoding system originally developed for boxcryptor's filename encryption.
It uses approx. 4000 Asian unicode characters to encode an array of bytes.

Why use it?
===========

When you store encoded data, sometimes you've got to deal with space limitations - e.g.
when working with third-party websites or cloud storage providers. The limit, however,
is determined by the amound of characters, not the memory necessary to store them.
Base4k "shrinks" its input data from n bytes to 2/3*n characters.

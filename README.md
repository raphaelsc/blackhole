blackhole
=========

Blackhole: A steganography program.

"Steganography is the art and science of writing hidden messages in such a way that no one, 
apart from the sender and intended recipient, suspects the existence of the message, 
a form of security through obscurity." From Wikipedia

usage
=========

raphaelsc@debian:~/blackhole$ ./blackhole README.md<br />
Generating obscure file: README.md.bh...<br />
Generating map file: README.md.bh.map...<br />

raphaelsc@debian:~/blackhole$ ./blackhole README.md.bh<br />
Generating output file: README.md.bh.out...<br />

raphaelsc@debian:~/blackhole$ ls -l<br />
...<br />
-rw-r--r-- 1 raphaelsc raphaelsc    57 Oct 25 01:20 README.md<br />
-r-------- 1 raphaelsc raphaelsc    33 Oct 25 06:03 README.md.bh<br />
-r-------- 1 raphaelsc raphaelsc    33 Oct 25 06:03 README.md.bh.map<br />
-r-------- 1 raphaelsc raphaelsc    57 Oct 25 06:03 README.md.bh.out<br />

.PHONY: all clean examples student_answers

all: examples student_answers

examples:
	+$(MAKE) -C examples/flock
	+$(MAKE) -C examples/mqueue
	+$(MAKE) -C examples/pipes
	+$(MAKE) -C examples/shmem
	+$(MAKE) -C examples/sockets
	+$(MAKE) -C examples/sigs

student_answers:
	+$(MAKE) -C student_answers/design_1
	+$(MAKE) -C student_answers/design_2
	+$(MAKE) -C student_answers/design_3
	+$(MAKE) -C student_answers/design_4
	+$(MAKE) -C student_answers/design_5
	+$(MAKE) -C student_answers/design_6
	+$(MAKE) -C student_answers/design_7
	+$(MAKE) -C student_answers/design_8
	+$(MAKE) -C student_answers/design_9_ultimate

clean:
	+$(MAKE) -C examples/flock clean
	+$(MAKE) -C examples/mqueue clean
	+$(MAKE) -C examples/pipes clean
	+$(MAKE) -C examples/shmem clean
	+$(MAKE) -C examples/sockets clean
	+$(MAKE) -C examples/sigs clean
	+$(MAKE) -C student_answers/design_1 clean
	+$(MAKE) -C student_answers/design_2 clean
	+$(MAKE) -C student_answers/design_3 clean
	+$(MAKE) -C student_answers/design_4 clean
	+$(MAKE) -C student_answers/design_5 clean
	+$(MAKE) -C student_answers/design_6 clean
	+$(MAKE) -C student_answers/design_7 clean
	+$(MAKE) -C student_answers/design_8 clean
	+$(MAKE) -C student_answers/design_9_ultimate clean


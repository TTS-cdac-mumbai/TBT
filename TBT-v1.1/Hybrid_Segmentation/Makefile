CXX=g++

PROGS=syllable_boundary_correction createMLF
.PHONY: clean
all:: $(PROGS)

syllable_boundary_correction: 
	$(CXX) src/syllable_boundary_correction.cpp -o syllable_boundary_correction

createMLF:
	$(CXX) src/createMLF.cpp -o hmm/createMLF
	$(CXX) src/createMLF_splice.cpp -o hmm/createMLF_splice
	$(CXX) src/createMLF_splice_2.cpp -o hmm/createMLF_splice_2

clean::
	-rm -f syllable_boundary_correction hmm/createMLF hmm/createMLF_splice hmm/createMLF_splice_2

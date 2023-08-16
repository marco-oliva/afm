[![Release](https://img.shields.io/github/v/release/marco-oliva/afm?include_prereleases)](https://github.com/marco-oliva/afm/releases/tag/1.0.0)

# Accelerated FM-index

FM-indexes are a crucial data structure in DNA alignment, but searching with them usually takes at least one random access per character in the query pattern.  Ferragina and Fischer observed in 2007 that word-based indexes often use fewer random accesses than character-based indexes, and thus support faster searches.  Since DNA lacks natural word-boundaries, however, it is necessary to parse it somehow before applying word-based FM-indexing.  Last year, Deng et al. proposed parsing genomic data by induced suffix sorting, and showed the resulting word-based FM-indexes support faster counting queries than standard FM-indexes when patterns are a few thousand characters or longer.  In this paper we show that using prefix-free parsing---which takes parameters that let us tune the average length of the phrases—instead of induced suffix sorting, gives a significant speedup for patterns of only a few hundred characters.  We implement our method and demonstrate it is between 3 and 18 times faster than competing methods on queries to GRCh38.  And was consistently faster on queries made to 25,000, 50,000 and 100,000 SARS-CoV-2 genomes. Hence, it is very clear that our method accelerates the performance of count over all state-of-the-art methods with a minor increase in the memory.

If you use the AFM in your research, please cite: (https://arxiv.org/abs/2305.05893)

Hong, Aaron., Oliva, Marco., Köppl, Dominik., Bannai, Hideo., Boucher, Christina., & Gagie, Travis. (2023). Acceleration of FM-index Queries Through Prefix-free Parsing. ArXiv. /abs/2305.05893

# Usage

## Build the PFP
We used the PFP implementation from the Boucher lab at the University of Florida. The GitHub link is: (https://github.com/marco-oliva/pfp). To build the PFP, run the following commands:
```
singularity pull pfp_sif docker://moliva3/pfp:latest
./pfp_sif pfp++ --help
```
For more details, please refer to the PFP GitHub page.

## Build the AFM
Then we can build the afm by running the following commands:
```
git clone https://github.com/marco-oliva/afm.git
cd afm
mkdir build
cd build
cmake ..
make
./afm -i input_file -w window-size -p modulo -n patterns-number -l patterns-length > output.txt
```

# Author
* Aaron Hong
* Marco Oliva
* Dominik Köppl
* Hideo Bannai
* Christina Boucher
* Travis Gagie

# Reference
[1] Boucher, C., Gagie, T., Kuhnle, A. et al. Prefix-free parsing for building big BWTs. Algorithms Mol Biol 14, 13 (2019). https://doi.org/10.1186/s13015-019-0148-5

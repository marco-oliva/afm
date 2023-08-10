[![Release](https://img.shields.io/github/v/release/marco-oliva/afm?include_prereleases)](https://github.com/marco-oliva/afm/releases/tag/1.0.0)

# afm
Accelerating FM-index queries with PFP

FM-index described in [1], built on prefix-free parsing compressed suffix tree [2].

If you use the AFM in your research, please cite:
arXiv:2305.05893v1

# Usage

## Build the PFP
We used the PFP implementation from Marco Oliva,the Github link is: (https://github.com/marco-oliva/pfp). To build the PFP, run the following commands:
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
[1] Paola Ferragina and Giovanni Manzini. Indexing Compressed Text. Journal of the ACM,513
52:552–581, 2005.

[2] Hong, Aaron., Oliva, Marco., Köppl, Dominik., Bannai, Hideo., Boucher, Christina., & Gagie, Travis. (2023). Acceleration of FM-index Queries Through Prefix-free Parsing. ArXiv. /abs/2305.05893

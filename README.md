Sample benchmark of `klib` and `qp-trie`. Results on my machine are as follows:

|               ns/op |    err% |          ins/op |          cyc/op |   miss% | geometric
|--------------------:|--------:|----------------:|----------------:|--------:|:----------
|        6,739,787.03 |    0.9% |   19,855,190.97 |   13,293,420.85 |    0.4% | `do nothing`
|       11,580,978.78 |    0.7% |   43,215,120.71 |   22,882,965.12 |    1.1% | `khash`
|       14,284,599.71 |    0.9% |   45,454,529.43 |   28,267,842.18 |    1.2% | `kbtree`
|       19,052,876.23 |    1.1% |   73,108,668.15 |   37,691,628.00 |    0.7% | `qptrie`

And for uniform input distribution:

|               ns/op |    err% |          ins/op |          cyc/op |   miss% | uniform
|--------------------:|--------:|----------------:|----------------:|--------:|:--------
|        5,210,641.06 |    0.3% |   18,830,762.62 |   10,286,954.09 |    0.0% | `do nothing`
|       30,790,390.12 |    2.1% |   60,865,541.84 |   56,536,040.76 |    7.3% | `khash`
|      121,925,907.91 |    0.6% |  169,881,847.80 |  240,909,897.24 |    8.6% | `kbtree`
|       63,831,198.88 |    0.4% |  134,903,808.19 |  122,556,872.35 |    2.2% | `qptrie`

To run the benchmark, run `make run_nb`.

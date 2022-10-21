#!/usr/bin/env python3
import re
from typing import Iterable
from dokusan import generators, solvers
from functools import reduce
from operator import add
import os


def djb2(elems: Iterable[int]) -> int:
    return reduce(lambda y, c: 0xFFFFFFFF & (y * 33 + c), elems, 5381)


def gen_input(field: Iterable[Iterable[int]]) -> str:
    return "\\n".join(map(lambda _: "".join(map(str, _)), field))


if __name__ == "__main__":
    os.system("echo -n > results.txt")
    for i in range(1000):
        sudoku = generators.random_sudoku(avg_rank=500)
        solution = solvers.backtrack(sudoku)
        solution_hash = djb2(map(lambda x: x.value, reduce(add, solution.rows())))
        print(f"Test {i}, \t\t\t\t   solution djb2 hash: 0x{str(hex(solution_hash)).upper()[2:]}")
        os.system(
            f"echo \"{gen_input(map(lambda x: map(lambda y: y.value or 0, x), sudoku.rows()))}\""
            f" | ./sudoku | head -n 1 | tee -a results.txt")
    with open("results.txt", "r") as f:
        print("95th percentile average time: {0:.3g}ms".format((
            (lambda z: sum(z) / len(z))
            ((lambda y: sorted(y)[:int(len(y) * 0.95)])
             ((lambda x: list(map(float, list(x))))
              (map(lambda w: re.search(r"took ([0-9.]+)ms", w).group(1), f.readlines()))))
        )))

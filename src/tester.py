import re
import shutil
import subprocess

import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns

CLI_TIME_REGEX = "Tempo cliente: (\d.\d+)"


def run_cli(input_):
    cli = subprocess.Popen(
        ["./client", "localhost"], stdout=subprocess.PIPE, stdin=subprocess.PIPE
    )

    stdout, stderr = cli.communicate(input_)

    out = stdout.decode("utf-8")

    time = float(re.findall(CLI_TIME_REGEX, out)[0])
    return time


def list_movies():
    input_ = b"7\n"
    return run_cli(input_)


def insert_movie():
    data = b"1\n1\nHarry Potter\nFantasia\n2\nUm filme muito legal com um carinha mtu gente boa"
    return run_cli(data)


def benchmark(repetitions, callable, callable_readable_name, draw_plot=True):
    results = np.array([callable() for _ in range(repetitions)])

    if draw_plot:
        ax = sns.boxplot(data=results)
        ax.set_title(
            f"Box plot do tempo de {repetitions} execuções de {callable_readable_name}"
        )
        plt.show()

    print(f"Resultados de {repetitions} execuções de {callable_readable_name}")

    print(f"Mean = {results.mean():.5f}")
    print(f"Median = {np.median(results):.5f}")
    print(f"Std Def = {results.std():.5f}")
    print(f"Max = {results.max():.5f}")
    print(f"Min = {results.min():.5f}")


def main(repetitions=200):
    benchmark(repetitions, insert_movie, "Inserção de Filmes")
    shutil.copy("demo_db.txt", "db.txt")
    benchmark(repetitions, list_movies, "Listagem de Filmes")


if __name__ == "__main__":
    main()

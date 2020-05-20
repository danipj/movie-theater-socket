import re
import subprocess

import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns

CLI_TIME_REGEX = "Tempo cliente: (\d.\d+)"


def list_movies():
    input_ = b"7\n"
    cli = subprocess.Popen(
        ["./client", "localhost"], stdout=subprocess.PIPE, stdin=subprocess.PIPE
    )

    stdout, stderr = cli.communicate(input_)

    out = stdout.decode("utf-8")

    time = float(re.findall(CLI_TIME_REGEX, out)[0])
    return time


def main(repetitions=1000):
    results = np.array([list_movies() for _ in range(repetitions)])
    sns.boxplot(data=results)
    plt.show()

    print(results.mean())
    print(results.std())


if __name__ == "__main__":
    main()

import numpy as np

x = open('nn_input.txt', 'r')
y = open('nn_output.txt', 'r')

x_lines = x.readlines()
y_lines = y.readlines()

weights = np.ones((160, 4)) * np.random.uniform(size=(160,4))

print(weights)

for x in range(len(x_lines)):
    x_lines[x] = x_lines[x].strip()
    y_lines[x] = y_lines[x].strip()


def train():
    for l in range(len(x_lines)):
        inp = x_lines[l].split(',')
        out1 = 0
        out2 = 0
        out3 = 0
        out4 = 0
        for x in range(len(inp)):
            out1 += weights[x][0] * inp[x]
            out2 += weights[x][1] * inp[x]
            out3 += weights[x][2] * inp[x]
            out4 += weights[x][3] * inp[x]

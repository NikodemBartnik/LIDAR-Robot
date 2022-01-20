
f1 = open('nn_input.txt')
data_input = []

for l in f1:
    data_input.append(l.strip().split(','))

f2 = open('nn_output.txt')
data_output = []

for l in f2:
    c = l.strip()
    data_output.append([1 if c == 'left' else 0, 1 if c == 'forward' else 0, 1 if c == 'right' else 0,])

print(len(data_output))


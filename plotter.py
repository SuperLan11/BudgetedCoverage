import random
import matplotlib.pyplot as plt

with open('output.txt') as data:
    lines = list(map(str.strip, data.read().split('\n')))

points = []
index = 1
for line in lines[index:]:
    index += 1
    if not line.startswith('('):
        break
    points.append(tuple(map(int, line.strip('()').split(', '))))

x_data = []
y_data = []
for x, y in points:
    x_data.append(x)
    y_data.append(y)

while not lines[index].isnumeric():
    index += 1

chosen = []
for line in lines[index:]:
    if line.isnumeric():
        chosen.append(int(line))
    else:
        break

chosen_points = []
for i in chosen:
    chosen_points.append(points[i])

chosen_x = []
chosen_y = []
for x, y in chosen_points:
    chosen_x.append(x)
    chosen_y.append(y)
    index += 1

while not lines[index].isnumeric():
    index += 1
R = int(lines[index])

colors = [(random.random() / 5) + 0.4 for i in range(len(chosen))]
area = R ** 2
print(area)

plt.ylim(0, 100)
plt.xlim(0, 100)
plt.scatter(chosen_x, chosen_y, marker='o', c=colors, s=[area]*len(chosen), alpha = 0.5)
plt.scatter(x_data, y_data, marker='X')
plt.show()
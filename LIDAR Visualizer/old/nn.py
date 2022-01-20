import tensorflow as tf
print('Tensorflow version:', tf.__version__)


f1 = open('nn_input.txt')
data_input = []

for l in f1:
    data_input.append(l.strip().split(','))

for l in range(len(data_input)):
    for p in range(len(data_input[0])):
      if p % 2 == 0:
        data_input[l][p] = float(data_input[l][p])/255
      else:
        data_input[l][p] = float(data_input[l][p])/8000

f2 = open('nn_output.txt')
data_output = []

for l in f2:
    c = l.strip()
    data_output.append([1 if c == 'left' else 0, 1 if c == 'forward' else 0, 1 if c == 'right' else 0,])

x_train = tf.convert_to_tensor(data_input)
y_train = tf.convert_to_tensor(data_output)

x_test = x_train
y_test = y_train

print('Shape: ', x_train.shape)

model = tf.keras.models.Sequential([
  tf.keras.layers.Dense(80, activation='relu', input_dim=(160)),
  tf.keras.layers.Dense(40, activation='relu'),
  tf.keras.layers.Dense(3)
])

predictions = model(x_train).numpy()
predictions

tf.nn.softmax(predictions).numpy()

model.compile(optimizer='adam',
              loss='categorical_crossentropy',
              metrics=['accuracy'])

model.fit(x_train, y_train, epochs=100)

model.evaluate(x_test,  y_test, verbose=5)

probability_model = tf.keras.Sequential([
  model,
  tf.keras.layers.Softmax()
])

print('\nResult: \n')
model.predict(data_input)
probability_model(x_test[:5])
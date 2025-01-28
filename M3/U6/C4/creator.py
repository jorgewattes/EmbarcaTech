import tensorflow as tf
import numpy as np
from tensorflow.keras import layers, models
from tensorflow.keras.utils import to_categorical
from tensorflow.keras.optimizers import Adam
import tensorflow_datasets as tfds

# 1. Carregar e processar o Google Speech Commands Dataset
dataset, info = tfds.load('speech_commands', with_info=True, as_supervised=True)

train_dataset = dataset['train']
val_dataset = dataset['validation']

# Filtros: Focar apenas na palavra "Alexa" e em sons de fundo
TARGET_WORDS = ["alexa", "background_noise"]
def filter_dataset(audio, label):
    word = info.features['label'].int2str(label)
    return word in TARGET_WORDS

train_dataset = train_dataset.filter(filter_dataset)
val_dataset = val_dataset.filter(filter_dataset)

# Pré-processamento: Converte áudio em espectrograma
def preprocess(audio, label):
    spectrogram = tf.signal.stft(audio, frame_length=256, frame_step=128)
    spectrogram = tf.abs(spectrogram)
    spectrogram = tf.expand_dims(spectrogram, -1)  # Adiciona canal
    label = tf.cast(label, tf.int32)
    return spectrogram, label

train_dataset = train_dataset.map(preprocess).batch(32).prefetch(1)
val_dataset = val_dataset.map(preprocess).batch(32).prefetch(1)

# 2. Construir a rede neural
model = models.Sequential([
    layers.Conv2D(32, (3, 3), activation='relu', input_shape=(None, None, 1)),
    layers.MaxPooling2D((2, 2)),
    layers.Conv2D(64, (3, 3), activation='relu'),
    layers.MaxPooling2D((2, 2)),
    layers.Flatten(),
    layers.Dense(128, activation='relu'),
    layers.Dense(len(TARGET_WORDS), activation='softmax')  # Saída para palavras-alvo
])

model.compile(optimizer=Adam(learning_rate=0.001), loss='sparse_categorical_crossentropy', metrics=['accuracy'])

# 3. Treinar o modelo
model.fit(train_dataset, validation_data=val_dataset, epochs=10)

# 4. Salvar o modelo e converter para TensorFlow Lite
model.save("alexa_model.h5")

# Converter para TFLite com quantização para uso em microcontroladores
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
tflite_model = converter.convert()

with open("alexa_model.tflite", "wb") as f:
    f.write(tflite_model)

'''
Modelo Treinado: O modelo alexa_model.tflite precisa ser convertido para um array C com o comando do TensorFlow:
xxd -i alexa_model.tflite > alexa_model_data.h
'''
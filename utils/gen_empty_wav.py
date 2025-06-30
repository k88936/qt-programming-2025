import wave

def generate_empty_wav(filename, sample_rate=44100):
    # Number of frames

    # Create a new wave file
    with wave.open(filename, 'w') as wav_file:
        # Set the parameters: 1 channel, 2 bytes per sample, sample rate
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_rate)

        # Write frames of silence
        wav_file.writeframes(b'\x00\x00')

# Generate an empty .wav file with 1 second duration
generate_empty_wav('empty.wav')
from gtts import gTTS
from pydub import AudioSegment

class TTS:
    def __init__(self, text_path="/static/prompt.txt"):
        with open(text_path, 'r', encoding='utf-8') as f:
            self.text = f.read()

    def text_to_speech(self):
        tts = gTTS(text=self.text, lang='de')
        tts.save("output.mp3")
        sound = AudioSegment.from_mp3("output.mp3")
        sound.export("output.wav", format="wav")

if __name__ == "__main__":
    tts_instance = TTS()
    tts_instance.text_to_speech()

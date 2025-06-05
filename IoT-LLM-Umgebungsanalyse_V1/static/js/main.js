document.getElementById("shot-btn").addEventListener("click", () => {
  document.getElementById("loading").style.display = "inline-block";
  fetch('/button_pressed', { method: 'POST' })
    .then(res => res.text())
    .then(() => {
      document.getElementById("screenshot-img").src =
        "/static/screenshots/screenshot.jpg?" + Date.now();
      setTimeout(fetchPrompt, 1000);
    })
    .finally(() => {
      document.getElementById("loading").style.display = "none";
    });
});

function fetchPrompt() {
  fetch('/get_prompt')
    .then(res => res.text())
    .then(txt => {
      document.getElementById("prompt-output").innerText = txt;
    });
}

// Prompt einmalig beim Laden holen
fetchPrompt();

const loadingProgress = document.getElementById("progress");
const loadingSpinner = document.getElementById("spinner");
const playButton = document.getElementById("play-button");
const status = document.getElementById("status");

function logMessage(msg) {
  console.log("[LOG]\t(main.js):\t" + msg);
}

Module = {
    noInitialRun: true,
    canvas: (function () {
      document.getElementById("canvas").addEventListener(
        "webglcontextlost",
        function (e) {
          e.preventDefault();
        },
        false
      );
  
      return document.getElementById("canvas");
    })(),
    setStatus: function (text) {
      if (!Module.setStatus.last) Module.setStatus.last = { 
        time: Date.now(), text: "" 
      };

      if (text === Module.setStatus.last.text) 
        return;
      
      const m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
      const now = Date.now();

      if (m && now - Module.setStatus.last.time < 30) 
        return; 

      Module.setStatus.last.time = now;
      Module.setStatus.last.text = text;

      if (m) {
        text = m[1];
        loadingProgress.value = parseInt(m[2]) * 100;
        loadingProgress.max = parseInt(m[4]) * 100;
        loadingProgress.hidden = false;
        loadingSpinner.hidden = false;
      } 
      else {
        loadingProgress.value = null;
        loadingProgress.max = null;
        loadingProgress.hidden = true;
        if (!text) loadingSpinner.hidden = true;
      }
      status.innerHTML = text;
  },
  running: false,
  selectedRom: [],
  loadSelectedRom: function () {
      this.ccall("load", null, ["array"], [this.selectedRom]);
  },
};

Module.setStatus("Loading...");

window.onerror = function () {
  Module.setStatus("An error occurred in the window, reloading the page");
  loadingSpinner.style.display = "none";
  setTimeout(function () {
      window.location.reload(true);
  }, 1000);
};

function getRomOptionsFromDropdown(optionText) {
  if (optionText === "Select a ROM") {
    return;
  }

  let romOptions = JSON.parse(optionText);
  const romName = romOptions["filename"];
  const romPath = "roms/" + romName + "\0"; 

  Module.selectedRom = new TextEncoder().encode(romPath);
  logMessage("Module.selectedRom = " + Module.selectedRom);

  playButton.disabled = false;

  Module.loadSelectedRom();
  logMessage("loaded the selected rom");
}

Module["onRuntimeInitialized"] = function () {
  getRomOptionsFromDropdown(document.querySelector("#rom-dropdown").value);

  document.querySelector("#rom-dropdown").onchange = function (event) {
      logMessage("ROM: " + event.target.value + " was selected");
      getRomOptionsFromDropdown(event.target.value);
  };

  playButton.addEventListener("click", () => {
    if (Module.running) {
      logMessage("stopping the emulator...");
      Module.ccall("stop", null, null, null);
      playButton.innerHTML = "Start";

      // reloading page to fully reset emulator display
      setTimeout(function () {  
        Module.running = false;
        window.location.reload(true);
      }, 500);
      logMessage("the emulator has stopped. press Start to play again");
    } 
    else {
      logMessage("starting the emulator...");
      Module.ccall("main", null, null, null);
      playButton.innerHTML = "Stop";
      Module.running = true;
      logMessage("the emulator has started. press Stop to stop playing");
    }
  });
};
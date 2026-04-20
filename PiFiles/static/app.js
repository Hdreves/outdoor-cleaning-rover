function setConnectedBadge(isConnected) {
    const badge = document.getElementById("connected_badge");

    if (isConnected) {
        badge.textContent = "online";
        badge.className = "badge online";
    } else {
        badge.textContent = "offline";
        badge.className = "badge offline";
    }
}

function setEstopBadge(isEstop) {
    const badge = document.getElementById("estop_badge");

    if (isEstop) {
        badge.textContent = "ACTIVE";
        badge.className = "badge estop-on";
    } else {
        badge.textContent = "inactive";
        badge.className = "badge estop-off";
    }
}

function formatDistance(value) {
    if (value === null || value === undefined) {
        return "--";
    }
    return `${value} mm`;
}

async function refreshStatus() {
    try {
        const response = await fetch("/api/status");
        const data = await response.json();

        document.getElementById("drive").textContent = data.drive ?? "stopped";
        document.getElementById("scoop").textContent = data.scoop ?? "idle";
        document.getElementById("mode").textContent = data.mode ?? "manual";
        document.getElementById("camera").textContent = data.camera ?? "offline";
        document.getElementById("last_command").textContent = data.last_command ?? "none";

        document.getElementById("left_ticks").textContent = data.left_ticks ?? 0;
        document.getElementById("right_ticks").textContent = data.right_ticks ?? 0;
        document.getElementById("tof_distance").textContent = formatDistance(data.tof_distance);

        setConnectedBadge(Boolean(data.connected));
        setEstopBadge(Boolean(data.estop));

        const cameraText = document.getElementById("camera_text");
        const cameraFeed = document.getElementById("camera_feed");

        if (cameraFeed && cameraText) {
            if (data.camera === "online") {
                cameraText.textContent = "Live camera stream";

                if (!cameraFeed.src || !cameraFeed.src.includes("/camera/stream")) {
                    cameraFeed.src = "/camera/stream";
                }

                cameraFeed.style.display = "block";
            } else {
                cameraText.textContent = "No stream connected yet";
                cameraFeed.removeAttribute("src");
                cameraFeed.style.display = "none";
            }
        }
    } catch (error) {
        console.error("Failed to refresh status:", error);
        setConnectedBadge(false);
    }
}

async function sendCommand(action, value = null) {
    try {
        const payload = { action: action };
        if (value !== null) {
            payload.value = value;
        }

        const response = await fetch("/api/control", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify(payload)
        });

        const data = await response.json();

        if (!response.ok) {
            alert(data.detail || data.error || "Command failed");
            return;
        }

        await refreshStatus();
    } catch (error) {
        console.error("Failed to send command:", error);
        alert("Could not reach rover backend");
    }
}

let activeDriveAction = null;
let activeScoopAction = null;

function startDrive(action) {
    if (activeDriveAction === action) {
        return;
    }
    activeDriveAction = action;
    sendCommand(action);
}

function stopDrive() {
    if (activeDriveAction === null) {
        return;
    }
    activeDriveAction = null;
    sendCommand("stop");
}

function startScoop(action) {
    if (activeScoopAction === action) {
        return;
    }
    activeScoopAction = action;
    sendCommand(action);
}

function stopScoop() {
    if (activeScoopAction === null) {
        return;
    }
    activeScoopAction = null;
    sendCommand("scoop_stop");
}

function bindHoldActionButton(buttonId, onStart, onStop) {
    const btn = document.getElementById(buttonId);
    if (!btn) return;

    btn.addEventListener("pointerdown", (event) => {
        event.preventDefault();
        onStart();
    });

    btn.addEventListener("pointerup", (event) => {
        event.preventDefault();
        onStop();
    });

    btn.addEventListener("pointerleave", () => {
        onStop();
    });

    btn.addEventListener("pointercancel", () => {
        onStop();
    });

    btn.addEventListener("contextmenu", (event) => {
        event.preventDefault();
    });
}

function isTypingIntoField(target) {
    if (!target) return false;

    const tagName = target.tagName?.toLowerCase();
    return (
        tagName === "input" ||
        tagName === "textarea" ||
        tagName === "select" ||
        target.isContentEditable
    );
}

document.addEventListener("pointerup", () => {
    stopDrive();
    stopScoop();
});

document.addEventListener("keydown", (event) => {
    if (isTypingIntoField(event.target) || event.repeat) {
        return;
    }

    const key = event.key.toLowerCase();

    if ([" ", "arrowup", "arrowdown", "arrowleft", "arrowright"].includes(key)) {
        event.preventDefault();
    }

    if (key === "w") {
        startDrive("forward");
    } else if (key === "a") {
        startDrive("left");
    } else if (key === "s") {
        startDrive("reverse");
    } else if (key === "d") {
        startDrive("right");
    } else if (key === "arrowup") {
        startScoop("scoop_up");
    } else if (key === "arrowdown") {
        startScoop("scoop_down");
    } else if (key === "arrowleft" || key === "arrowright") {
        startScoop("dump");
    } else if (key === " ") {
        stopDrive();
        stopScoop();
    } else if (key === "e") {
        sendCommand("emergency_stop");
    }
});

document.addEventListener("keyup", (event) => {
    if (isTypingIntoField(event.target)) {
        return;
    }

    const key = event.key.toLowerCase();

    if (["w", "a", "s", "d"].includes(key)) {
        stopDrive();
    }

    if (["arrowup", "arrowdown", "arrowleft", "arrowright"].includes(key)) {
        stopScoop();
    }
});

bindHoldActionButton("btn-forward", () => startDrive("forward"), stopDrive);
bindHoldActionButton("btn-left", () => startDrive("left"), stopDrive);
bindHoldActionButton("btn-right", () => startDrive("right"), stopDrive);
bindHoldActionButton("btn-reverse", () => startDrive("reverse"), stopDrive);

bindHoldActionButton("btn-scoop-up", () => startScoop("scoop_up"), stopScoop);
bindHoldActionButton("btn-scoop-down", () => startScoop("scoop_down"), stopScoop);
bindHoldActionButton("btn-dump", () => startScoop("dump"), stopScoop);

sendCommand("stop");
sendCommand("scoop_stop");
refreshStatus();
setInterval(refreshStatus, 1000);

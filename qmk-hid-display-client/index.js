const HID = require('node-hid');
const loudness = require('loudness');
var os = require('os-utils');

const KEYBOARD_NAME = "keeb_proto"; // Example product ID
const KEYBOARD_USAGE_ID =  0x61; // 97 for rawhid
const KEYBOARD_USAGE_PAGE = 0xFF60; // 65376 for rawhid
const KEYBOARD_UPDATE_TIME = 1000;
let keyboard = null;
let volume = 0;
let cpu = 0;
let memory = 0;

function updateKeyboardScreen() {
    if (!keyboard) {
        const devices = HID.devices();
        for (const d of devices) {
            if (d.product === KEYBOARD_NAME && d.usage === KEYBOARD_USAGE_ID && d.usagePage === KEYBOARD_USAGE_PAGE) {
                // Create a new connection and store it as the keyboard
                keyboard = new HID.HID(d.path);
                console.log(`Keyboard connection established.`);
                break;
            }
        }
    }
    
    if (keyboard) {
        getSystemVolume();
        os.cpuUsage(function(v){
            cpu = Math.round(v*1000);
        });

        memory = Math.round(100 - os.freememPercentage()*100);

        try {
            // Listen for data from the keyboard which indicates the screen to show
            // keyboard.on('data', (e) => {
            //     console.log(e)
            // });

            // Send the report
            const text = "Demo"; // Text you want to send
            const textBuffer = Buffer.from(text, 'utf8');
            const report = [0, 4, ...textBuffer];

            keyboard.write(report);
            keyboard.write([0, 1, volume]);
            keyboard.write([0, 2, cpu]);
            keyboard.write([0, 3, memory]);
            //console.log('Report sent');
        } catch {
            console.log("failed")
        }
    }
}

async function getSystemVolume() {
    try {
        volume = await loudness.getVolume();
        //console.log(`Current system volume: ${volume}%`);
    } catch (err) {
        console.error('Error getting system volume:', err);
    }
}

setInterval(updateKeyboardScreen, KEYBOARD_UPDATE_TIME);
import subprocess
import sys
import os
import xml.etree.ElementTree as ET


MULTIMONITORTOOL = os.path.join("dependencies", "multimonitortool.exe")
MONITORS_XML = os.path.join("dependencies", "monitors.xml")


def create_monitors_xml():
    try:
        subprocess.run([MULTIMONITORTOOL, "/sxml", MONITORS_XML], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error creating monitors XML: {e}")
        sys.exit(1)


def parse_monitors_xml(xml_path):
    """Parse the XML file and extract monitor information."""
    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()
        monitors = [
            (
                item.find("name").text,
                item.find("monitor_name").text,
                item.find("active").text,
                item.find("primary").text,
            )
            for item in root.findall("item")
        ]
        return monitors
    except ET.ParseError as e:
        print(f"Error parsing monitors XML: {e}")
        sys.exit(1)


def generate_monitors():
    create_monitors_xml()
    return parse_monitors_xml(MONITORS_XML)


def send_ddcci_on(monitor_names):
    monitors = generate_monitors()
    for monitor in monitors:
        monitor_name = monitor[1]
        if monitor_name in monitor_names:
            try:
                monitor_index = monitor[0]
                subprocess.run([MULTIMONITORTOOL, "/TurnOn", monitor_index], check=True)
                print(f"Enabled monitor: {monitor_name}")
            except subprocess.CalledProcessError as e:
                print(f"Error enabling monitor {monitor_name}: {e}")


def send_ddcci_off(monitor_names):
    monitors = generate_monitors()
    for monitor in monitors:
        monitor_name = monitor[1]
        if monitor_name in monitor_names:
            try:
                monitor_index = monitor[0]
                subprocess.run([MULTIMONITORTOOL, "/TurnOff", monitor_index], check=True)
                print(f"Disabled monitor: {monitor_name}")
            except subprocess.CalledProcessError as e:
                print(f"Error disabling monitor {monitor_name}: {e}")


def send_displayswitch(argument):
    try:
        subprocess.run(["displayswitch.exe", argument], check=True)
        print(f"Switched to {argument} display.")
    except subprocess.CalledProcessError as e:
        print(f"Error switching to {argument} display: {e}")


def list_monitors():
    monitors = generate_monitors()
    lines = [f"Monitor: {monitor[1]}, Active: {monitor[2]}, Primary: {monitor[3]}" for monitor in monitors]

    try:
        with open("monitors.txt", "w") as file:
            file.write("\n".join(lines))
        print("Monitor information has been written to monitors.txt")
    except IOError as e:
        print(f"Error writing to file monitors.txt: {e}")

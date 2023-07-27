#!/bin/bash

# Check if the script is running as root
if [ "$(id -u)" -ne 0 ]; then
  # If not running as root, restart the script with elevated privileges using sudo
  echo "This script requires root privileges. Restarting with sudo..."
  sudo "$0" "$@"
  exit $?
fi

# Define the paths
# Get the path to kio/servicemenus using qtpaths
servicemenus_path=$(qtpaths --locate-dirs GenericDataLocation kio/servicemenus)

# Check if the ServiceMenus directory exists
if [ ! -d "$servicemenus_path" ]; then
  echo "Creating $servicemenus_path..."
  mkdir -p "$servicemenus_path"
fi

select_action() {
    echo "Press Key to select:"
    echo "[1]: Install"
    echo "[2]: Uninstall"
    read -n 1 key
    echo ""

    if [ "$key" = "1" ]; then
        echo "Installing..."
        install
    elif [ "$key" = "2" ]; then
        echo "Uninstalling..."
        uninstall
    else
        echo "You pressed '$key'. Not a recognized key. Press either [1] or [2]."
        select_action
    fi
}

install() {
    script_dir="$(cd "$(dirname "$0")" && pwd)"
    cd "$servicemenus_path"
    command_on_convert="sh -c \"python3 $script_dir/convert.py '"%U"'\""
    file="convert.desktop"
    filecontent="[Desktop Entry]\nType=Service\nMimeType=application/octet-stream;\nActions=convert\n\n[Desktop Action convert]\nName=Convert File\nIcon=inode-x-generic\nExec=$command_on_convert"
    echo -e "$filecontent" > "$file"
    chmod +x "$file"
    exit_script
}

uninstall() {
    cd "$servicemenus_path"
    rm "./convert.desktop"
    exit_script
}

exit_script() {
    echo "Done. Press any key to exit."
    read -n 1
    exit 0
}

select_action

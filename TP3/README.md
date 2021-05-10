# TP3

Commandes utiles :
```
candump vcan2
```
sudo modprobe vcan # Load virtual CAN Kernel module
sudo ip link add dev vcan0 type vcan # Add virtual CAN socket called vcan0
sudo ip link set up vcan0 # Set vcan0 up

sudo ip link add dev vcan1 type vcan # Add virtual CAN socket called vcan1
sudo ip link set up vcan1 # Set vcan1 up

sudo ip link add dev vcan2 type vcan # Add virtual CAN socket called vcan2
sudo ip link set up vcan2 mtu 72
sudo ip link set down vcan0
sudo ip link set up vcan0 mtu 72

source venv_SAdT/bin/activate
python -m avsim2D
```
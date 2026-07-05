#include <bus.h>
// Abstraccion importante porque el cpu le escribe al bus, que es el que finalmente se encarga 
// de "comunicarse" con el dispositivo correspondiente a la direccion accedida

u8 busRead(u16 addr)
{
    // Que belleza
    device_t* device;
    if (getDMATransferState() == true) 
    {
        // Si hay transferencia DMA -> OAM, entonces el bus esta inhabilitado para el cpu exceptuando la HRAM
        device = getDeviceByIndex(hramId);
        if (device->enRango(addr))
        {
            return device->read(addr);
        } else {
            return -1;
        }
    } else {
        for(int i = 0; i < cantDevices; i++)
        {
            device = getDeviceByIndex(i);
            if (device->enRango(addr))
            {
                return device->read(addr);
            }
        }
    }

    return -1;
}

void busWrite(u16 addr, u8 val)
{
    // Que belleza X2
    device_t* device;
    if (getDMATransferState() == true) 
    {
        // Si hay transferencia DMA -> OAM, entonces el bus esta inhabilitado para el cpu exceptuando la HRAM
        device = getDeviceByIndex(hramId);
        if (device->enRango(addr))
        {
            device->write(addr, val);
        }
    } 
    else
    {
        for(int i = 0; i < cantDevices; i++)
        {
            device = getDeviceByIndex(i);
            if (device->enRango(addr))
            {
                device->write(addr, val);
            }
        }
    }

}

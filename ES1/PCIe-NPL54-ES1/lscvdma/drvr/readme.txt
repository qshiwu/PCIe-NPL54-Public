1. debug 

cd /proc/driver
sudo su

// read VDMA_VERSION Register
echo ver >lscvdma
cat lscvdma

// read VDMA_C2H_CR Register
echo cr >lscvdma
cat lscvdma


// read VDMA_C2H_SR Register
echo sr >lscvdma
cat lscvdma

// read VDMA_C2H_IRQ_MASK Register 
echo irqmask >lscvdma
cat lscvdma

// read VDMA_C2H_IRQ_SRC Register 
echo irqsrc >lscvdma
cat lscvdma

// clr VDMA_C2H_IRQ_SRC Register 
echo clr >lscvdma
cat lscvdma

dmesg

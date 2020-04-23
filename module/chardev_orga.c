#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/errno.h>

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev_orga"
#define BUF_LEN 90

static int Major_Number;
static int Device_Open = 0;

static char Message[BUF_LEN];
static char *Message_Ptr;

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/* Cargar el modulo y obtener Major Number */
int init_module(void)
{
    Major_Number = register_chrdev(0, DEVICE_NAME, &fops);

    if(Major_Number < 0){
        printk(KERN_ALERT "Error al registrar char device con %d.\n", Major_Number);
        return Major_Number;
    }

    printk(KERN_INFO "Se cargó el modulo %s.\n", DEVICE_NAME);
    printk(KERN_INFO "Se le asignó el Major Number %d.\n", Major_Number);
    printk(KERN_INFO "Debe crear un archivo dev con \n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major_Number);
    
    /*Ver para crear con device_create(), mknod()*/

    return SUCCESS;
}

/* Anular registro del device */
void cleanup_module(void)
{    
    unregister_chrdev(Major_Number, DEVICE_NAME);

    printk("Quitando modulo %s.\n", DEVICE_NAME);
}

/* Metodos */

static int device_open(struct inode *inode, struct file *file)
{
    if(Device_Open) 
        return -EBUSY;

    Device_Open++;
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
    Device_Open--;
    module_put(THIS_MODULE);

    return SUCCESS;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset){
    int bytes_read = 0;
    
    if(*Message_Ptr == 0)
        return 0;

    while(length && *Message_Ptr){
        put_user(*(Message_Ptr++), buffer++);
        length--;
        bytes_read++;
    }
   
    return bytes_read;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    int index;

    for(index = 0; index < len && index < BUF_LEN; index++){
        get_user(Message[index], buff + index);
    }
    Message_Ptr = Message;
    printk(KERN_INFO "Se escribió en /dev/%s: %s\n", DEVICE_NAME, Message_Ptr);
    return index;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PABLO ABREGU");
MODULE_DESCRIPTION("Kernel_Module_ORGAII");

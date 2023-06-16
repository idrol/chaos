# Virtual filesystem

typedef uint32_t ino_t;
typedef uint32_t dev_t; 

#define DT_UNKNOWN 0x0
#define DT_REG     0x1
#define DT_DIR     0x2

struct dirent_t {
  char[] name;
  uint8_t nameLength;
  uint8_t type;
  ino_t st_ino; // File serial number (inode num)
  dev_t st_dev; // Device id (logical device)
};

struct stat_t {
  
};
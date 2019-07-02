//
//  EXT2fs.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 02/07/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include "EXT2fs.h"

static FSModule _ext2fs;
FSModule* ext2fs = &_ext2fs;

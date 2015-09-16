#!/usr/bin/perl

use strict;
use warnings;

#Number of sectors to load MUST BE <=0xFF!

my $LOAD_SEG_ADDRESS="0030";

my $SECTOR_SIZE=0x200;

my $BOOT_NOF_SECTORS_OFFSET="2";
my $BOOT_LOAD_SEG_ADDRESS_OFFSET="6";
my $ENTRY32_KERNEL_SIZE_OFFSET="5";
my $ENTRY32_KERNEL_CRC_OFFSET=$ENTRY32_KERNEL_SIZE_OFFSET+4;

my $OFOL="./output";

my $ENTRY32="$OFOL/entry32.bin";
my $BOOT="$OFOL/boot.bin";

my $FSIZE="./build_utils/fsize.pl";
my $PATCHER="./build_utils/patcher";
my $CRC="./build_utils/crc";

#-----------------------------------------

-w $ENTRY32 and -w $BOOT or die("can't patch files");

my $kernel_size_le=`$FSIZE -le .8X ./output/entry32.bin`;
die("fsize failed") if($?);

print "kernel size le: $kernel_size_le\n";

my $load_sectors=`$FSIZE u ./output/entry32.bin`;
die("fsize failed") if($?);

$load_sectors=($load_sectors+0x100)/$SECTOR_SIZE+1;

$load_sectors=sprintf("%X",$load_sectors);

if(length($load_sectors)==1){$load_sectors="0".$load_sectors;}

`$PATCHER $OFOL/boot.bin $BOOT_NOF_SECTORS_OFFSET $load_sectors $BOOT_LOAD_SEG_ADDRESS_OFFSET $LOAD_SEG_ADDRESS`;
print "patcher failed: $!" if($?);

`$PATCHER $OFOL/entry32.bin $ENTRY32_KERNEL_SIZE_OFFSET $kernel_size_le`;
print "patcher failed: $!" if($?);

#-----------------------------------------

my $kernel_crc=`$CRC $OFOL/entry32.bin`;

`$PATCHER $OFOL/entry32.bin $ENTRY32_KERNEL_CRC_OFFSET $kernel_crc`;


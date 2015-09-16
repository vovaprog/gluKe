#!/usr/bin/perl

use strict;
use warnings;

my $UPDATE=2;

sub bad_usage{
	print <<EOTEXT;
NAME
	fsize perl script update $UPDATE.	
USAGE
	fsize.pl [--little-endian] format file_name1 [file_name2 ...]
DESCRIPTION
	Script outputs size in bytes of each file.
	If program fails to open any of files - error is returned.
OPTIONS
	--little-endian (or short -le) - convert output: for example 
        	                         '12ABCDEF' to 'EFCDAB12'.
	format                         - printf \%-conversion for file size.
	file_name1 file_name2          - file sizes will be printed to stdout 
                                         separated with spaces.
EOTEXT
	exit -1;
}

bad_usage() if(scalar(@ARGV)<2);

my $format=shift || bad_usage();

my $le=($format eq '--little-endian' or $format eq '-le')?1:0;
$format=shift || bad_usage() if($le);

my $file;
foreach $file (@ARGV)
{
	my $fh;
	if(open($fh,"<$file"))
	{		
		my $output=sprintf("%$format",sysseek($fh,0,2));
		close($fh);
		if($le && length($output) % 2==0)
		{
			$output=scalar(reverse($output));
			my $i;
			for($i=0;$i<length($output);$i+=2)
			{
				substr($output,$i,2)=scalar(reverse(substr($output,$i,2)));
			}			
		}
		print $output.(scalar(@ARGV)==1?"":" ");
	}
	else
	{
		die("Can't open file $file\n");
	}		
}


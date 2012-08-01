#!/usr/bin/perl

use strict;
use warnings;
use Cwd;
use Getopt::Long;
use 5.010;

my $DEBUG = 0;

GetOptions (
    'debug'  => \$DEBUG,
);

my @paths;
if (@ARGV) {
    foreach my $input_path (@ARGV) {
        if (-d $input_path) {
            $input_path =~ s{(?<!/)$}{/};
            push @paths, $input_path;
        }
    }
}
else {
    push @paths, getcwd().'/';
}

my %hash_files;
foreach my $path (@paths) {
    foreach my $file (map($path.$_, file_list($path))) {
        my $hash_code = `cat "$file" 2>/dev/null | sha1sum | sed -n -e '2d' -e 's/ .*\$//p'`;
        push @{$hash_files{$hash_code}}, $file;
    }
}

foreach my $file_hash (keys %hash_files) {
    print join("\n", (@{$hash_files{$file_hash}}))."\n\n" if @{$hash_files{$file_hash}} > 1;
}

sub file_list {
    my $path = shift;

    my @dirs = ($path);
    my @file_list;
    my ($dir, $file);
    while ($dir = pop(@dirs)) {
        local *DH;

        if (!opendir(DH, $dir)) {
            warn "Cannot opendir $dir: $!";
            next;
        }

        foreach (readdir(DH)) {
            if ($_ eq '.' || $_ eq '..') {
                next;
            }

            $file = $dir.$_;
            if (-d $file) {
                $file .= '/';
                push @dirs, $file ;
            }
            else {
                my $rel_file = $file;
                $rel_file =~ s/$path//;
                push @file_list, $rel_file;
            }
        }
        closedir(DH);
    }

    @file_list;
}

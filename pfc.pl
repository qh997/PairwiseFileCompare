#!/usr/bin/perl

use strict;
use warnings;
use Cwd;
use Getopt::Long;
use Digest::SHA qw(sha1_hex);
use 5.010;

my $HASH = 0;
my $DEBUG = 0;

GetOptions (
    'h|hash' => \$HASH,
    'debug' => \$DEBUG,
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
        open my $FH, $file or next;
        local $/="";
        my $file_content = <$FH>;
        close $FH;

        $file_content = $file_content ? $file_content : "";
        my $hash_code = sha1_hex($file_content);
        push @{$hash_files{$hash_code}}, $file;
    }
}

foreach my $file_hash (keys %hash_files) {
    print "$file_hash\n" if $HASH && @{$hash_files{$file_hash}} > 1;
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

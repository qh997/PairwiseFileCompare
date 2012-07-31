#!/usr/bin/perl

use strict;
use warnings;
use 5.010;
use Cwd;

my $ORI_PATH = @ARGV ? shift @ARGV : getcwd;
my $PRE_PATH = @ARGV ? shift @ARGV : $ORI_PATH;
$ORI_PATH =~ s{(?<!/)$}{/};
$PRE_PATH =~ s{(?<!/)$}{/};

my @ori_files = file_list($ORI_PATH);
my @pre_files = $ORI_PATH eq $PRE_PATH ? @ori_files : file_list($PRE_PATH);

print "Compare ".@ori_files." files to ".@pre_files." files.\n";

my %same_files;

foreach my $ori_file (@ori_files) {
    my $o_file = $ORI_PATH.$ori_file;
    my $o_file_size = -s $o_file;

    foreach my $pre_file (@pre_files) {
        my $p_file = $PRE_PATH.$pre_file;
        next if $p_file eq $o_file;
        my $p_file_size = -s $p_file;

        if ($o_file_size and $p_file_size and $o_file_size == $p_file_size) {
            my $o_file_hash = `cat $o_file 2>/dev/null | sha1sum`;
            $o_file_hash =~ s/\s.*//s;
            my $p_file_hash = `cat $p_file 2>/dev/null | sha1sum`;
            $p_file_hash =~ s/\s.*//s;

            if ($o_file_hash eq $p_file_hash) {
                push @{$same_files{$o_file_hash}}, $o_file unless grep($_ eq $o_file, @{$same_files{$o_file_hash}});
                push @{$same_files{$o_file_hash}}, $p_file unless grep($_ eq $p_file, @{$same_files{$o_file_hash}});
            }
        }
    }
}

foreach my $file_hash (keys %same_files) {
    print join("\n", (@{$same_files{$file_hash}}))."\n\n";
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
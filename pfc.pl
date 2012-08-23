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
        my $sha = Digest::SHA->new();

        open my $FH, $file or next;
        $sha->addfile($FH);
        my $hash_code = $sha->hexdigest;
        close $FH;

        push @{$hash_files{$hash_code}}, $file;
    }
}

my %results;
foreach my $file_hash (keys %hash_files) {
    if (@{$hash_files{$file_hash}} > 1) {
        my @sorted_files = sort @{$hash_files{$file_hash}};
        @{$results{$file_hash}} = (@sorted_files);
    }
}

my @hash_list = sort { $results{$a}->[0] cmp $results{$b}->[0] } keys %results;

foreach my $hash (@hash_list) {
    print "$hash\n" if $HASH;
    print join("\n", (@{$results{$hash}}))."\n\n";
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
            $file = $dir.$_;

            next if ($_ eq '.' || $_ eq '..' || -l $file);

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

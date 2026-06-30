#!/usr/bin/perl
#This fuzzer checks a bunch of syntax combinations and sees if they agree with 
#perl's math engine 
#This file is licensed under MIT-0
#Copyright 2026 Ribbon-otter
use warnings;
use strict;
use List::Util qw(first);

my $evalm_path = first {-f $_}  ('./evalm', './build/evalm');

my $test_successful = "yes";

# this system isn't efficient, since it starts a new process for each generated
# forumla. However, it is good enough since I can just wait.
for my $i (0 .. 1_000_000) {
    $_ = $i;
    tr#0245678#.+\-*/()#;
    my $evalm = `$evalm_path '$_' 2> /dev/null`;
    chomp $evalm;
    if ($? == 0) {
        my $evalp;
        {
            local $SIG{__WARN__} = sub { }; 
            $evalp = eval "$_";
        }
        unless (defined($evalp)) {next;}
        unless ($evalp == $evalm) {
            print "oh no: '$_' evalm '$evalm' vs perl '$evalp'\n";
            $test_successful = "no";
        }
    }
}
print("was the test successful? $test_successful \n");
# vim: ts=4 sw=4 expandtab

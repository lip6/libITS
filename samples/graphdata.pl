#!/usr/bin/perl -w
use Getopt::Std;
use strict;

if ($#ARGV <= 3)
{
    print STDERR "syntax: graphdata.pl [-v] meth1 meth2 column files...\n";
    print STDERR "\n  -v distinguish verdicts instead of models";
    exit(2);
}

my $opt_v = 0;
if ($ARGV[0] eq '-v')
{
    $opt_v = 1;
    shift @ARGV;
}

my %verdict = ( 0 => "empty", 1 => "non-empty", 2 => "unknown" );

my $opt_x = shift @ARGV;
my $opt_y = shift @ARGV;
my $opt_c = (shift @ARGV) - 3;


my $max = 0;

my $count = 0;
my $fail = 0;

my $head = <>;
my @head = split(',', $head);
shift @head;
shift @head;
shift @head;

my %result;

while (<>)
{
    next if ($_ =~ /^\w*$/) or ($_ eq $head);

    chomp;

    my @res = split(',',$_);
    my $meth = shift @res;
    my $model = shift @res;
    my $formula = shift @res;
    if (defined $res[$opt_c] && $res[$opt_c] !~ /^\s*$/) {
	if ($res[$opt_c] > $max) {
	    $max = $res[$opt_c];
	}
    } else {
      $res[$opt_c] = -1;
    }

    $result{"$model,$formula"}{$meth} = [@res];
}

foreach my $key (keys %result)
{
    if (defined $result{$key}{$opt_x}
	and defined $result{$key}{$opt_y})
    {
	my $t1 = $result{$key}{$opt_x};
	my $val1 = $t1->[$opt_c];
	if ((! defined $val1) || $val1 =~ /^\s*$/ || $val1 == -1 ) {
	    $val1 = 3* $max;
	}
	my $t2 = $result{$key}{$opt_y};
	my $val2 = $t2->[$opt_c];
	if ((! defined $val2) || $val1 =~ /^\s*$/ || $val2 == -1) {
	    $val2 = 3* $max;
	}

	my $model;

	if ($opt_v)
	{
	    my $v1 = int $t1->[3];
	    my $v2 = int $t2->[3];
	    $v1 = $v2 if ($v1 > $v2);
	    $model = $verdict{$v1};
	}
	else
	{
	    ($model = $key) =~ s,.*?/?([^/]*).net.*,$1,;
	    $model =~ s/\dnm/-nm/;
	    $model =~ s/\d+//;
	}
	print "$model $val1 $val2\n";
	if ($val1 > 100 * $val2) {
	  print STDERR "Over 2 orders of magnitude : $opt_x :$val1 > $opt_y :$val2 FOR  $key\n";
	} elsif ( $val2 > 100 * $val1) {
	  print STDERR "Over 2 orders of magnitude : $opt_y :$val2 > $opt_x :$val1  FOR  $key\n";
	}

    }
}

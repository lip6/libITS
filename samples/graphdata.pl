#!/usr/bin/perl -w
use Getopt::Std;

if ($#ARGV <= 3)
{
    print STDERR "syntax: graphdata.pl meth1 meth2 column files...\n";
    exit(2);
}

my $opt_x = shift @ARGV;
my $opt_y = shift @ARGV;
my $opt_c = (shift @ARGV) - 3;

my @sum;
my @max;
my @min;
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

    $result{"$model,$formula"}{$meth} = [@res];
}

foreach my $key (keys %result)
{
    if (defined $result{$key}{$opt_x}
	and defined $result{$key}{$opt_y})
    {
	my $t1 = $result{$key}{$opt_x};
	my $val1 = $t1->[$opt_c];
	my $t2 = $result{$key}{$opt_y};
	my $val2 = $t2->[$opt_c];
	print "$val1 $val2\n";
    }
}

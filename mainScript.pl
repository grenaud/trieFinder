#!/usr/bin/perl


use strict;
use warnings;
use Getopt::Std;
use Data::Dumper;
use File::Basename;

my $fakeRun       = 0;

#sub systemBash {
#  my $cmd = shift;
#  my @args = ( "bash", "-c", $cmd );
#  print "command ".$cmd."\n";
#  if(!$fakeRun){
#    system(@args);
#    if($? != 0){
#      die "Command ".Dumper(@args)." failed";
#    }
#  }
#}
sub findAllPIDs{
 my ($firstPID) = @_;
 my %hashPID;
 $hashPID{$firstPID}=1;
 my $found;
 do{
   $found=0;
   foreach my $key (keys %hashPID){
     my $cmd="ps -o pid --ppid ".$key;
     my $out=`$cmd`;
     foreach my $line (split("\n",$out)){
       $line =~ s/^\s+//;
       $line =~ s/\s+$//;
       if($line ne "PID"){
         if(!(exists $hashPID{$line})){
           $hashPID{$line}=1;
           $found=1;
         }
       }
     }
   }

 }while($found);
 return (keys %hashPID);
}

sub poll{
 my $proc=shift;
 while(1){
   my $cmd="ps  -o s --pid $proc";
   my $out=`$cmd`;
   my $found=0;
   my @arr=split("\n",$out);
   shift(@arr);
   foreach my $line (@arr){
     $line =~ s/^\s+//;
     $line =~ s/\s+$//;
     if($line ne "Z"  ){
       $found=1;
     }
   }
   if(!$found){
     return $proc;
   }
   sleep(2);
 }
}

sub systemBash {
 my $cmd = shift;
 my @args = ( "bash", "-c", $cmd );

 print "running ".$cmd."\n";
 if(!$fakeRun){
   my $pid=fork();
   if($pid == 0){
     exec(@args);
   }else{
     sleep(1);
     my @array=findAllPIDs($pid);
     foreach my $pid (@array){
       my $code =poll($pid);
     }
   }
 }
}

my $maxNumberOfLines=15000000;

my $length=75;
my $mismatchesRestSize=1;
my $restSequence  = "none";
my $outdir        = "none";
my $refseq        = "none";
my $unigene       = "none";
my $genomic       = "none";
my $refseqFN      = "none";
my $unigeneFN     = "none";
my $genomicFN     = "none";
my $inputFile     = "none";

my $allowedMismatches=1;

my %opts;

if(!(-e "src/allTag")){
  die "The allTag program must be built and in the src/ directory\n";
}

if(!(-e "src/readFiles.pl")){
  die "The readFiles.pl program must be built in the src/ directory\n";
}

if(!(-e "src/searchtags")){
  die "The searchtags program must be built and in the src/ directory\n";
}

if(!(-e "src/mergeAnnotation.pl")){
  die "The mergeAnnotation.pl program must be built in the src/ directory\n";
}



my $usage=
  "$0 [options]\n\n".
  "Main script to automatically build the database, parse and search for the tags\n\n".
  "Database building options\n".
  "\t-l Length of sequence for database             (Default:".$length.")\n".
  "\t-n Allowed mismatches in restriction site      (Default:".$mismatchesRestSize.")\n".
  "\t-r Restriction sequence to use e.g. GATC       (Default:".$restSequence.")\n".
  "\t-o Directory to store the database files       (Default:".$outdir.")\n".
  "\t-s Path to the RefSeq database  (fasta format) (Default:".$refseq.")\n".
  "\t-u Path to the Unigene database (fasta format) (Default:".$unigene.")\n".
  "\t-g Path to the Genomic database (fasta format) (Default:".$genomic.")\n".
  "\nTag searching options\n".
  "\t-m Allowed mismatches during search                          (Default:".$allowedMismatches.")\n".
  "\t-i Path to the input file containing the tags (fastq format) (Default:".$inputFile.")\n".
  "\t   make sure you can write in that directory, create a symlink otherwise\n".
  "\t-f Fake run, just print commands used\n".
  "\n";


if($#ARGV == -1 || ($#ARGV==0 && ($ARGV[0] eq "-h" || $ARGV[0] eq "--help" ))){
  die $usage;
}

getopts('l:n:r:o:s:u:g:m:i:f:', \%opts);


if(exists $opts{'i'}){
  $inputFile=$opts{'i'};
  if(!(-e $inputFile)){
    die "Error, input file = ".$inputFile." does not exist\n";
  }
}else{
  die "-i option must be specified\n";
}


if(exists $opts{'g'}){
  $genomic=$opts{'g'};
  if(!(-e $genomic)){
    die "Error, genomic database = ".$genomic." does not exist\n";
  }
  my ($fn,$dir,$suffix)=fileparse($genomic);
  $genomicFN=$fn.$suffix;
}else{
  die "-g option must be specified\n";
}



if(exists $opts{'u'}){
  $unigene=$opts{'u'};
  if(!(-e $unigene)){
    die "Error, unigene database = ".$unigene." does not exist\n";
  }
  my ($fn,$dir,$suffix)=fileparse($unigene);
  $unigeneFN=$fn.$suffix;
}else{
  die "-u option must be specified\n";
}

if(exists $opts{'s'}){
  $refseq=$opts{'s'};
  if(!(-e $refseq)){
    die "Error, refseq database = ".$refseq." does not exist\n";
  }

  my ($fn,$dir,$suffix)=fileparse($refseq);
  $refseqFN=$fn.$suffix;

}else{
  die "-s option must be specified\n";
}


if(exists $opts{'o'}){
  $outdir=$opts{'o'};

  if($outdir !~ /\/$/){
    $outdir.="/";
  }

  if(!(-d $outdir)){
    die "directory to store the database files does not exists\n";
  }

}else{
  die "-o option must be specified\n";
}



if(exists $opts{'l'}){
  $length=$opts{'l'};
  if($length != /^\d+$/){
    die "-l parameter must be numerical";
  }
}else{

}

if(exists $opts{'n'}){
  $mismatchesRestSize=$opts{'n'};
  if($mismatchesRestSize != /^\d+$/){
    die "-n parameter must be numerical";
  }
}else{

}


if(exists $opts{'m'}){
  $allowedMismatches=$opts{'m'};
  if($mismatchesRestSize != /^\d+$/){
    die "-n parameter must be numerical";
  }
}else{

}


if(exists $opts{'r'}){
  $restSequence=$opts{'r'};
}else{
  die "-r option must be specified\n";
}



if(exists $opts{'f'}){
  $fakeRun=1
}

print "Summary of options:\n".
  "Length of sequence for database        :\t".$length."\n".
  "Allowed mismatches in restriction site :\t".$mismatchesRestSize."\n".
  "Restriction sequence to use            :\t".$restSequence."\n".
  "Directory to store the database files  :\t".$outdir."\n".
  "Path to the RefSeq database            :\t".$refseq."\n".
  "Path to the Unigene database           :\t".$unigene."\n".
  "Path to the Genomic database           :\t".$genomic."\n".
  "Filename to the RefSeq database        :\t".$refseqFN."\n".
  "Filename to the Unigene database       :\t".$unigeneFN."\n".
  "Filename to the Genomic database       :\t".$genomicFN."\n".
  "Allowed mismatches search              :\t".$allowedMismatches."\n";



if ( (-e $outdir.$refseqFN.".".$restSequence.".l".$length.".mm0.out.gz") &&
     (-e $outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outA.gz") &&
     (-e $outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outC.gz") &&
     (-e $outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outG.gz") &&
     (-e $outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outT.gz") ){
  print "Refseq files found :\n";
  print $outdir.$refseqFN.".".$restSequence.".l".$length.".mm0.out.gz"."\n";
  print $outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outA.gz"."\n";
  print $outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outC.gz"."\n";
  print $outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outG.gz"."\n";
  print $outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outT.gz"."\n";
}else{
  my $cmd="src/allTag  -l 75 -m 1 -r ".$restSequence." -d $refseq  -o >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm0.out.gz)   -p1 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outA.gz)   -p2 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outC.gz)   -p3 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outG.gz)   -p4 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outT.gz)";
  systemBash($cmd);
#  `$cmd`;
#  if($? != 0){
#    die "Command ".$cmd." failed";
#  }

}




if ( (-e $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm0.out.gz") &&
     (-e $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outA.gz") &&
     (-e $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outC.gz") &&
     (-e $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outG.gz") &&
     (-e $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outT.gz") ){
  print "Unigene files found :\n";
  print $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm0.out.gz"."\n";
  print $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outA.gz"."\n";
  print $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outC.gz"."\n";
  print $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outG.gz"."\n";
  print $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outT.gz"."\n";
}else{
  my $cmd="src/allTag  -l 75 -m 1 -r ".$restSequence." -d $unigene  -o >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm0.out.gz)   -p1 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outA.gz)   -p2 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outC.gz)   -p3 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outG.gz)   -p4 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outT.gz)";
  #print $cmd;
  #`$cmd`;
  #if($? != 0){
  #  die "Command ".$cmd." failed";
  #}
  systemBash($cmd);
}


if ( (-e $outdir.$genomicFN.".".$restSequence.".l".$length.".mm0.out.gz") &&
     (-e $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outA.gz") &&
     (-e $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outC.gz") &&
     (-e $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outG.gz") &&
     (-e $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outT.gz") ){
  print "Genomic files found :\n";
  print $outdir.$genomicFN.".".$restSequence.".l".$length.".mm0.out.gz"."\n";
  print $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outA.gz"."\n";
  print $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outC.gz"."\n";
  print $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outG.gz"."\n";
  print $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outT.gz"."\n";
}else{
  my $cmd="src/allTag  -l 75 -m 1 -r ".$restSequence." -d $genomic  -o >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm0.out.gz)   -p1 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outA.gz)   -p2 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outC.gz)   -p3 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outG.gz)   -p4 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outT.gz)";
  #print $cmd;
  #`$cmd`;
  #if($? != 0){
  #  die "Command ".$cmd." failed";
  #}
  systemBash($cmd);
}




if ( (-e $outdir."all".".".$restSequence.".l".$length.".mm0.out") ){
  print "Found ".$outdir."all".".".$restSequence.".l".$length.".mm0.out\n";
}else{
  my $cmd="src/readFiles.pl ".$restSequence." A 0  ".$refseqFN." ".$unigeneFN." ".$genomicFN." $outdir $length  > ".$outdir."all.".$restSequence.".l".$length.".mm0.out";
  #`$cmd`;
  #if($? != 0){
  #  die "Command ".$cmd." failed";
  #}
  systemBash($cmd);
}

if ( (-e $outdir."all".".".$restSequence.".l".$length.".mm1.outA") ){
  print "Found ".$outdir."all".".".$restSequence.".l".$length.".mm1.outA\n";
}else{
  my $cmd="src/readFiles.pl ".$restSequence." A 1 ".$refseqFN." ".$unigeneFN." ".$genomicFN." $outdir $length > ".$outdir."all.".$restSequence.".l".$length.".mm1.outA";
  #`$cmd`;
  #if($? != 0){
  #  die "Command ".$cmd." failed";
  #}
  systemBash($cmd);

}

if ( (-e $outdir."all".".".$restSequence.".l".$length.".mm1.outC") ){
  print "Found ".$outdir."all".".".$restSequence.".l".$length.".mm1.outC\n";
}else{
  my $cmd="src/readFiles.pl ".$restSequence." C 1 ".$refseqFN." ".$unigeneFN." ".$genomicFN." $outdir $length > ".$outdir."all.".$restSequence.".l".$length.".mm1.outC";
  #`$cmd`;
  #if($? != 0){
  #  die "Command ".$cmd." failed";
  #}
  systemBash($cmd);

}

if ( (-e $outdir."all".".".$restSequence.".l".$length.".mm1.outG") ){
  print "Found ".$outdir."all".".".$restSequence.".l".$length.".mm1.outG\n";
}else{
  my $cmd="src/readFiles.pl ".$restSequence." G 1 ".$refseqFN." ".$unigeneFN." ".$genomicFN." $outdir $length > ".$outdir."all.".$restSequence.".l".$length.".mm1.outG";
#  `$cmd`;
#  if($? != 0){
#    die "Command ".$cmd." failed";
#  }
  systemBash($cmd);

}

if ( (-e $outdir."all".".".$restSequence.".l".$length.".mm1.outT") ){
  print "Found ".$outdir."all".".".$restSequence.".l".$length.".mm1.outT\n";
}else{
  my $cmd="src/readFiles.pl ".$restSequence." T 1 ".$refseqFN." ".$unigeneFN." ".$genomicFN." $outdir $length > ".$outdir."all.".$restSequence.".l".$length.".mm1.outT";
#  print $cmd;
#  `$cmd`;
#  if($? != 0){
#    die "Command ".$cmd." failed";
#  }
    systemBash($cmd);

}

my $lastDbIndex;
if ( (-e $outdir."all".".".$restSequence.".l".$length.".db.0") ){
  my $dbIndex=0;

  while((-e $outdir."all".".".$restSequence.".l".$length.".db.".$dbIndex ) ){
    print "Found ".$outdir."all".".".$restSequence.".l".$length.".db.".$dbIndex."\n";
    $lastDbIndex=$dbIndex;
    $dbIndex++;
  }

} else {

  my $line;
  my $lineNumber=0;

  my $dbIndex=0;
  open(FILEOUT,">".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex) or die "Cannot open ".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex." for writing\n";
  print "Writing to ".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex."\n";

  foreach my $file ($outdir."all.".$restSequence.".l".$length.".mm0.out",
		    $outdir."all.".$restSequence.".l".$length.".mm1.outA",
		    $outdir."all.".$restSequence.".l".$length.".mm1.outC",
		    $outdir."all.".$restSequence.".l".$length.".mm1.outG",
		    $outdir."all.".$restSequence.".l".$length.".mm1.outT") {
    open(FILE,$file) or die "Cannot open ".$file."\n";
    while ($line =<FILE>) {
      print FILEOUT $line;
      $lineNumber++;
      if ($lineNumber == 15000000) {
	$lineNumber=0;
	$dbIndex++;
	$lastDbIndex=$dbIndex;
	open(FILEOUT,">".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex) or die "Cannot open ".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex." for writing\n";
	print "Writing to ".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex."\n";
      }
    }
    close(FILE);
    #print $file."\n";
  }

  close(FILEOUT);

}

for(my $dbindex=0;$dbindex<=$lastDbIndex;$dbindex++){
  my $cmd="src/searchtags ".$outdir."all".".".$restSequence.".l".$length.".db.".$dbindex." ".$inputFile." ".$allowedMismatches." > ".$inputFile.".out".$dbindex."\n";
  #print "Running command= ".$cmd."\n";
  #`$cmd`;
  #if($? != 0){
  #  die "Command ".$cmd." failed";
  #}
  systemBash($cmd);

}

my @filesToMerge;
for(my $dbindex=0;$dbindex<=$lastDbIndex;$dbindex++){
  push(@filesToMerge,$inputFile.".out".$dbindex);
}
my $cmd="src/mergeAnnotation.pl ".join(" ",@filesToMerge)." > ".$inputFile.".out";

#`$cmd`;
#if($? != 0){
#  die "Command ".$cmd." failed";
#}
systemBash($cmd);


for(my $dbindex=0;$dbindex<=$lastDbIndex;$dbindex++){
  print "Removing ".$inputFile.".out".$dbindex."\n";
  unlink($inputFile.".out".$dbindex);
}

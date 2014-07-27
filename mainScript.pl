#!/usr/bin/perl


use strict;
use warnings;
use Getopt::Std;
use Data::Dumper;
use File::Basename;

#use IO::Compress::Gzip qw(gzip $GzipError) ;
use IO::Zlib;

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

#ugly hack due to the fact that we need to wait for processes writing to file descriptor
#rather than actual files recursively finds all PIDs where the parent is the
#given as argument
sub findAllPIDs{
 my ($firstPID) = @_;
 my %hashPID;
 $hashPID{$firstPID}=1;
 my $found;
 do{
   $found=0;
   foreach my $key (keys %hashPID){
     my $cmd="ps -f  -A 2> /dev/null";
     my $out=`$cmd`;

     if($? != 0){
       $cmd="ps -f  -a  2> /dev/null";
       $out=`$cmd`;
     }
     $out =~ s/^\s+//;
     $out =~ s/\s+$//;


     my @allLines  = split("\n",$out);
     my $firstLine = shift(@allLines);

     my @fieldFirstLine = split(/\s+/,$firstLine);

     if($fieldFirstLine[1] ne  "PID"){
       die "The ps command does not follow UNIX standards, please use a MAC or Linux system fields = ".Dumper(@fieldFirstLine);
     }
     if($fieldFirstLine[2] ne  "PPID"){
       die "The ps command does not follow UNIX standards, please use a MAC or Linux system fiels  = ".Dumper(@fieldFirstLine);
     }

     foreach my $line (@allLines){
       $line =~ s/^\s+//;
       $line =~ s/\s+$//;

       my @fields = split(/\s+/,$line);
       if($fields[2] eq $key){ #if parent ID is the one sought
	 if(!(exists $hashPID{$fields[1]})){
           $hashPID{$fields[1]}=1;
           $found=1;
         }
       }
     }


   }

 }while($found);
 return (keys %hashPID);
}


#check if a process exists in the process table
#and waits for it to die
sub poll{
 my $proc=shift;
 while(1){
   my $cmd="ps  -f -A 2> /dev/null ";
   my $out=`$cmd`;

   if($? != 0){
     $cmd="ps -f  -a  2> /dev/null ";
     $out=`$cmd`;
   }
   $out =~ s/^\s+//;
   $out =~ s/\s+$//;



   my $found=0;
   my @arr=split("\n",$out);

   my $firstLine = shift(@arr); #header
   my @fieldFirstLine = split(/\s+/,$firstLine);

   if($fieldFirstLine[1] ne  "PID"){
     die "The ps command does not follow UNIX standards, please use a MAC or Linux system fields =".Dumper(@fieldFirstLine);
   }
   if($fieldFirstLine[2] ne  "PPID"){
     die "The ps command does not follow UNIX standards, please use a MAC or Linux system fiels  = ".Dumper(@fieldFirstLine);
   }



   foreach my $line (@arr){
     $line =~ s/^\s+//;
     $line =~ s/\s+$//;
     my @fields = split(/\s+/,$line);
     #print Dumper(@fields);
     #print $proc;
     #print "#".$fields[1]."#\n";
     if($fields[1] eq $proc){
       if($fields[$#fields] ne "<defunct>"){ #check for zombie
	 #print "found ";
	 $found=1;
       }
     }
   }
   if(!$found){
     #print "not found ".$proc." exiting\n";
     return $proc;
   }else{
     #print "found ".$proc." looping\n";
   }
   sleep(2);
 }
}

sub systemBash {
 my $cmd = shift;
 my @args = ( "bash", "-c", $cmd );

 warn "\nrunning ".$cmd."\n";
 if(!$fakeRun){
   my $pid=fork();
   if($pid == 0){
     exec(@args);
   }else{
     sleep(1);
     #print "PID of child $pid\n";
     my @array=findAllPIDs($pid);
     #print "PID to wait for :".Dumper(@array);
     foreach my $pid (@array){
       my $code =poll($pid);
     }
   }
 }
}

my $maxNumberOfLines=15000000;

my $length=76;
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
my $onlyCreateDatabase=0;

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
  "(by default at most 1 mismatch in the recognition site)\n".
  "\t-l Length of sequence for database             (Default:".$length.")\n".
  #  "\t-n Allowed mismatches in restriction site      (Default:".$mismatchesRestSize.")\n".
  "\t-r Restriction sequence to use e.g. GATC       (Default:".$restSequence.")\n".
  "\t-o Directory to store the database files       (Default:".$outdir.")\n".
  "\t-s Path to the RefSeq database  (fasta format) (Default:".$refseq.")\n".
  "\t-u Path to the Unigene database (fasta format) (Default:".$unigene.")\n".
  "\t-g Path to the Genomic database (fasta format) (Default:".$genomic.")\n".
  "\t-d Only build the database\n".

  "\nTag searching options\n".
  "\t-m Allowed mismatches during search                          (Default:".$allowedMismatches.")\n".
  "\t-i Path to the input file containing the tags (fastq format) (Default:".$inputFile.")\n".
  "\t   make sure you can write in that directory, create a symlink otherwise\n".
  "\t-f Fake run, just print commands used\n".
  "\n";


if($#ARGV == -1 || ($#ARGV==0 && ($ARGV[0] eq "-h" || $ARGV[0] eq "--help" ))){
  die $usage;
}

getopts('l:r:o:s:u:g:m:i:fd', \%opts);

if(exists $opts{'d'}){
  $onlyCreateDatabase=1;
}



if(exists $opts{'i'}){
  $inputFile=$opts{'i'};
  if(!(-e $inputFile)){
    die "Error, input file = ".$inputFile." does not exist\n";
  }
}else{
  if($onlyCreateDatabase != 1){
    die "-i option must be specified\n";
  }
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
  if($length !~ /^\d+$/){
    die "-l parameter must be numerical";
  }
}else{

}

#if(exists $opts{'n'}){
#  $mismatchesRestSize=$opts{'n'};
#  if($mismatchesRestSize != /^\d+$/){
#    die "-n parameter must be numerical";
#  }
#}else{
#}


if(exists $opts{'m'}){
  if($onlyCreateDatabase == 1){
    die "Do not specify both -m and -d";
  }

  $allowedMismatches=$opts{'m'};
  print "allowedMismatches $allowedMismatches\n";
  if($allowedMismatches !~ /^\d+$/){
    die "-m parameter must be numerical";
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


warn "Summary of options:\n".
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
  warn "Refseq files found :\n";
  warn $outdir.$refseqFN.".".$restSequence.".l".$length.".mm0.out.gz"."\n";
  warn $outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outA.gz"."\n";
  warn $outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outC.gz"."\n";
  warn $outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outG.gz"."\n";
  warn $outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outT.gz"."\n";
}else{
#  my $cmd="src/allTag  -l ".$length." -m 1 -r ".$restSequence." -d $refseq  -o >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm0.out.gz)   -p1 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outA.gz)   -p2 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outC.gz)   -p3 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outG.gz)   -p4 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outT.gz)";
  my $cmd="src/allTag  -l ".$length." -m 1 -r ".$restSequence." -d $refseq  -o ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm0.out.gz    -p1  ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outA.gz   -p2  ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outC.gz   -p3  ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outG.gz   -p4  ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outT.gz";
  systemBash($cmd);

  $cmd="zcat ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm0.out.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm0.out.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm0.out.sort.gz ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm0.out.gz";
  systemBash($cmd);


  $cmd="zcat ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outA.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outA.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outA.sort.gz ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outA.gz";
  systemBash($cmd);


  $cmd="zcat ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outC.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outC.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outC.sort.gz ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outC.gz";
  systemBash($cmd);


  $cmd="zcat ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outG.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outG.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outG.sort.gz ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outG.gz";
  systemBash($cmd);


  $cmd="zcat ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outT.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outT.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outT.sort.gz ".$outdir.$refseqFN.".".$restSequence.".l".$length.".mm1.outT.gz";
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
  warn "Unigene files found :\n";
  warn $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm0.out.gz"."\n";
  warn $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outA.gz"."\n";
  warn $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outC.gz"."\n";
  warn $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outG.gz"."\n";
  warn $outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outT.gz"."\n";
}else{
#  my $cmd="src/allTag  -l ".$length." -m 1 -r ".$restSequence." -d $unigene  -o >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm0.out.gz)   -p1 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outA.gz)   -p2 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outC.gz)   -p3 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outG.gz)   -p4 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outT.gz)";
  my $cmd="src/allTag  -l ".$length." -m 1 -r ".$restSequence." -d $unigene  -o  ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm0.out.gz   -p1  ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outA.gz   -p2  ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outC.gz   -p3  ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outG.gz   -p4  ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outT.gz";
  #print $cmd;
  #`$cmd`;
  #if($? != 0){
  #  die "Command ".$cmd." failed";
  #}
  systemBash($cmd);


  $cmd="zcat ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm0.out.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm0.out.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm0.out.sort.gz ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm0.out.gz";
  systemBash($cmd);


  $cmd="zcat ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outA.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outA.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outA.sort.gz ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outA.gz";
  systemBash($cmd);


  $cmd="zcat ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outC.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outC.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outC.sort.gz ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outC.gz";
  systemBash($cmd);


  $cmd="zcat ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outG.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outG.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outG.sort.gz ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outG.gz";
  systemBash($cmd);


  $cmd="zcat ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outT.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outT.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outT.sort.gz ".$outdir.$unigeneFN.".".$restSequence.".l".$length.".mm1.outT.gz";
  systemBash($cmd);





}


if ( (-e $outdir.$genomicFN.".".$restSequence.".l".$length.".mm0.out.gz") &&
     (-e $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outA.gz") &&
     (-e $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outC.gz") &&
     (-e $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outG.gz") &&
     (-e $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outT.gz") ){
  warn "Genomic files found :\n";
  warn $outdir.$genomicFN.".".$restSequence.".l".$length.".mm0.out.gz"."\n";
  warn $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outA.gz"."\n";
  warn $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outC.gz"."\n";
  warn $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outG.gz"."\n";
  warn $outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outT.gz"."\n";
}else{
  #my $cmd="src/allTag  -l ".$length." -m 1 -r ".$restSequence." -d $genomic  -o >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm0.out.gz)   -p1 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outA.gz)   -p2 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outC.gz)   -p3 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outG.gz)   -p4 >(sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip -c > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outT.gz)";
  my $cmd="src/allTag  -l ".$length." -m 1 -r ".$restSequence." -d $genomic  -o  ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm0.out.gz   -p1  ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outA.gz   -p2  ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outC.gz   -p3  ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outG.gz   -p4  ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outT.gz";
  #print $cmd;
  #`$cmd`;
  #if($? != 0){
  #  die "Command ".$cmd." failed";
  #}
  systemBash($cmd);



  $cmd="zcat ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm0.out.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm0.out.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm0.out.sort.gz ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm0.out.gz";
  systemBash($cmd);


  $cmd="zcat ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outA.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outA.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outA.sort.gz ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outA.gz";
  systemBash($cmd);


  $cmd="zcat ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outC.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outC.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outC.sort.gz ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outC.gz";
  systemBash($cmd);


  $cmd="zcat ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outG.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outG.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outG.sort.gz ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outG.gz";
  systemBash($cmd);


  $cmd="zcat ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outT.gz | sort --temporary-directory=".$outdir." -t \$'\\t' -k3,3 - | gzip > ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outT.sort.gz";
  systemBash($cmd);
  $cmd="/bin/mv -f  ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outT.sort.gz ".$outdir.$genomicFN.".".$restSequence.".l".$length.".mm1.outT.gz";
  systemBash($cmd);


}




if ( (-e $outdir."all".".".$restSequence.".l".$length.".mm0.out.gz") ){
  warn "Found ".$outdir."all".".".$restSequence.".l".$length.".mm0.out.gz\n";
}else{
  my $cmd="src/readFiles.pl ".$restSequence." A 0  ".$refseqFN." ".$unigeneFN." ".$genomicFN." $outdir $length | gzip   > ".$outdir."all.".$restSequence.".l".$length.".mm0.out.gz";
  #`$cmd`;
  #if($? != 0){
  #  die "Command ".$cmd." failed";
  #}
  systemBash($cmd);
}

if ( (-e $outdir."all".".".$restSequence.".l".$length.".mm1.outA.gz") ){
  warn "Found ".$outdir."all".".".$restSequence.".l".$length.".mm1.outA.gz\n";
}else{
  my $cmd="src/readFiles.pl ".$restSequence." A 1 ".$refseqFN." ".$unigeneFN." ".$genomicFN." $outdir $length | gzip  > ".$outdir."all.".$restSequence.".l".$length.".mm1.outA.gz";
  #`$cmd`;
  #if($? != 0){
  #  die "Command ".$cmd." failed";
  #}
  systemBash($cmd);

}

if ( (-e $outdir."all".".".$restSequence.".l".$length.".mm1.outC.gz") ){
  warn "Found ".$outdir."all".".".$restSequence.".l".$length.".mm1.outC.gz\n";
}else{
  my $cmd="src/readFiles.pl ".$restSequence." C 1 ".$refseqFN." ".$unigeneFN." ".$genomicFN." $outdir $length  | gzip  > ".$outdir."all.".$restSequence.".l".$length.".mm1.outC.gz";
  #`$cmd`;
  #if($? != 0){
  #  die "Command ".$cmd." failed";
  #}
  systemBash($cmd);

}

if ( (-e $outdir."all".".".$restSequence.".l".$length.".mm1.outG.gz") ){
  warn "Found ".$outdir."all".".".$restSequence.".l".$length.".mm1.outG.gz\n";
}else{
  my $cmd="src/readFiles.pl ".$restSequence." G 1 ".$refseqFN." ".$unigeneFN." ".$genomicFN." $outdir $length  | gzip  > ".$outdir."all.".$restSequence.".l".$length.".mm1.outG.gz";
  #  `$cmd`;
  #  if($? != 0){
  #    die "Command ".$cmd." failed";
  #  }
  systemBash($cmd);

}

if ( (-e $outdir."all".".".$restSequence.".l".$length.".mm1.outT.gz") ){
  warn "Found ".$outdir."all".".".$restSequence.".l".$length.".mm1.outT.gz\n";
}else{
  my $cmd="src/readFiles.pl ".$restSequence." T 1 ".$refseqFN." ".$unigeneFN." ".$genomicFN." $outdir $length  | gzip  > ".$outdir."all.".$restSequence.".l".$length.".mm1.outT.gz";
  #  print $cmd;
  #  `$cmd`;
  #  if($? != 0){
  #    die "Command ".$cmd." failed";
  #  }
  systemBash($cmd);
}



#die;
my $lastDbIndex;
if ( (-e $outdir."all".".".$restSequence.".l".$length.".db.0.gz") ){
  my $dbIndex=0;

  while((-e $outdir."all".".".$restSequence.".l".$length.".db.".$dbIndex.".gz" ) ){
    warn "Found ".$outdir."all".".".$restSequence.".l".$length.".db.".$dbIndex.".gz\n";
    $lastDbIndex=$dbIndex;
    $dbIndex++;
  }

} else {

  my $line;
  my $lineNumber=0;

  my $dbIndex=0;

  my @arrayOfFilesToCombine = ($outdir."all.".$restSequence.".l".$length.".mm0.out.gz",
			       $outdir."all.".$restSequence.".l".$length.".mm1.outA.gz",
			       $outdir."all.".$restSequence.".l".$length.".mm1.outC.gz",
			       $outdir."all.".$restSequence.".l".$length.".mm1.outG.gz",
			       $outdir."all.".$restSequence.".l".$length.".mm1.outT.gz");
  if (!$fakeRun) {

    #open(FILEOUT,">".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex.".gz") or die "Cannot open ".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex.".gz for writing\n";
    #my $FILEOUT = new IO::Zlib; 
    #my $FILEOUT = IO::Zlib->new( $outdir."all.".$restSequence.".l".$length.".db.".$dbIndex.".gz" , "rb");
    open(FILEOUT," | gzip > ".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex.".gz") or die "Cannot pipe to ".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex.".gz for writing\n";
    #if( !(defined $FILEOUT) ){
    warn "Writing to ".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex.".gz\n";
    $lastDbIndex=0;

    foreach my $file (@arrayOfFilesToCombine) {
      open(FILE,"gunzip -c $file |") || die "can't open pipe to $file\n";
      while ($line =<FILE>) {
	print FILEOUT $line;
	$lineNumber++;
	if ($lineNumber == 15000000) {
	  $lineNumber=0;
	  $dbIndex++;
	  $lastDbIndex=$dbIndex;
	  open(FILEOUT," | gzip > ".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex.".gz") or die "Cannot pipe to ".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex." for writing\n";
	  warn "Writing to ".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex.".gz\n";
	}
      }
      close(FILE);
      #print $file."\n";
    }

    close(FILEOUT);
    #$FILEOUT->close;

  }else{
    warn "Combining files :\n";
    foreach my $file (@arrayOfFilesToCombine) {
      warn $file."\n";
    }
    warn "into ".$outdir."all.".$restSequence.".l".$length.".db.".$dbIndex.".gz\n";
    $lastDbIndex=1;
  }
}


#die $lastDbIndex;

if($onlyCreateDatabase == 0){


  for(my $dbindex=0;$dbindex<=$lastDbIndex;$dbindex++){
    my $cmd="src/searchtags ".$outdir."all".".".$restSequence.".l".$length.".db.".$dbindex.".gz ".$inputFile." ".$allowedMismatches." > ".$inputFile.".out".$dbindex."\n";
    systemBash($cmd);
  }

  my @filesToMerge;
  for(my $dbindex=0;$dbindex<=$lastDbIndex;$dbindex++){
    push(@filesToMerge,$inputFile.".out".$dbindex);
  }
  my $cmd="src/mergeAnnotation.pl ".join(" ",@filesToMerge)." > ".$inputFile.".out";

  systemBash($cmd);


  for(my $dbindex=0;$dbindex<=$lastDbIndex;$dbindex++){
    warn "Removing ".$inputFile.".out".$dbindex."\n";
    unlink($inputFile.".out".$dbindex);
  }

}

warn "Program finished successfully\n";



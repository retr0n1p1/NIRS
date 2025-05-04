#rawtoroot<br>
Use: `bash run.sh -e 1000 -q data.sql`<br>
- flag -e 1000    process only 1000 entries<br>
- flag -q         disable progress-bar<br>
- flag -n 5000    set nint filter<br>
If used without flags, the program will check that all events have the same nint; otherwise, it will create multiple trees, each with its own nint.

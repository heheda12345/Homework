testcases = ["test", "joy"];
for i = 1: length(testcases)
    fprintf("Testcase: " + testcases(i) + "\n");
    ans_fft = find_keys("data/" + testcases(i) + ".m4a", @impl_fft, testcases(i) + "-fft");
    fprintf("fft output: " + ans_fft + "\n");
    ans_goertzel = find_keys("data/" + testcases(i) + ".m4a", @impl_goertzel, testcases(i) + "-goertzel");
    fprintf("goertzel output: " + ans_goertzel + "\n");
    fprintf("\n");
end
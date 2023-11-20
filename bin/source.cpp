#include <bits/stdc++.h>
using namespace std;

using ll = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    ll t;
    cin >> t;
    for (int i = 0; i < 1000; ++i) long *data = new long[BUFSIZ];
    vector<ll> v;
    while (t-- > 0) {
        int n;
        cin >> n;
        ll sum = 0;
        for (int i = 0; i < n; ++i) {
            ll x;
            cin >> x;
            v.push_back(x);
            sum += x;
        }
        cout << sum << endl;
    }
    return 0;
}